#include "graphics/DeferredRenderer.h"
#include "graphics/Debug.h"
#include "util/Logging.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::map<GLenum,std::string> GL_ERROR_STRINGS = {
    {GL_INVALID_ENUM, "GL_INVALID_ENUM"},
    {GL_INVALID_VALUE, "GL_INVALID_VALUE"},
    {GL_INVALID_OPERATION, "GL_INVALID_OPERATION"},
    {GL_STACK_OVERFLOW, "GL_STACK_OVERFLOW"},
    {GL_STACK_UNDERFLOW, "GL_STACK_UNDERFLOW"},
    {GL_OUT_OF_MEMORY, "GL_OUT_OF_MEMORY"},
    {GL_INVALID_FRAMEBUFFER_OPERATION, "GL_INVALID_FRAMEBUFFER_OPERATION"}
};


DeferredRenderer::DeferredRenderer(bool enabledDebugRendering)
    : debugRenderingEnabled(enabledDebugRendering)
{

}

DeferredRenderer::~DeferredRenderer()
{

}

void DeferredRenderer::init (float width, float height, bool softInitialise)
{
    screenWidth = GLsizei(width);
    screenHeight = GLsizei(height);

    if (! softInitialise) {
        gbufferSpriteShader = Shader::load("data/shaders/sprites.vert", "data/shaders/sprites.frag");
        pbrLightingShader = Shader::load("data/shaders/deferredlighting.vert", "data/shaders/pbr.frag");
        gbufferBackgroundShader = Shader::load("data/shaders/background.vert", "data/shaders/background.frag");

        u_texture = gbufferBackgroundShader.uniform("u_texture");

        // Retrieve uniform locations
//        u_gbuffer_rendermode = gbufferShader.uniform("renderMode");

        if (debugRenderingEnabled) {
            debugShader = Shader::load("data/shaders/debug.vert", "data/shaders/debug.frag");
            u_debugTexture = debugShader.uniform("debugTexture");
            u_debugMode = debugShader.uniform("debugMode");
        }

        // Fullscreen quad
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // Setup UBO for matrices
        glGenBuffers(1, &matrices_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        // Connect UBO to binding point 0
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrices_ubo, 0, 2 * sizeof(glm::mat4));

        // Connect shader UBO blocks to binding point 0
        gbufferSpriteShader.bindUnfiromBlock("Matrices", 0);
        gbufferBackgroundShader.bindUnfiromBlock("Matrices", 0);

        // Setup renderables
        spritePool.init(gbufferSpriteShader);
    }


    // Setup projection
    projection_matrix = glm::perspective(glm::radians(60.0f), width / height, 0.1f, 20.0f);

    // Load projection into UBO
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection_matrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Create Framebuffer Object
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Create render targets
    // - position color buffer (position vec3 + AO)
    glGenTextures(1, &gBufferPosition);
    glBindTexture(GL_TEXTURE_2D, gBufferPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBufferPosition, 0);

    // - normal color buffer (normal vec3 + roughness)
    glGenTextures(1, &gBufferNormal);
    glBindTexture(GL_TEXTURE_2D, gBufferNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBufferNormal, 0);

    // - albedo buffer (albedo rgb + specular)
    glGenTextures(1, &gBufferAlbedo);
    glBindTexture(GL_TEXTURE_2D, gBufferAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screenWidth, screenHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBufferAlbedo, 0);

    // Attach render targets to framebuffer object
    Buffer_t attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // create and attach depth buffer
    glGenRenderbuffers(1, &gBufferDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gBufferDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBufferDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        warn("Framebuffer not complete!");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST);
    glClearDepth(1.0);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void DeferredRenderer::term (bool softTerminate)
{
    glDeleteBuffers(1, &matrices_ubo);
    glDeleteBuffers(1, &gBuffer);
    glDeleteBuffers(1, &gBufferDepth);
    glDeleteBuffers(1, &gBufferPosition);
    glDeleteBuffers(1, &gBufferNormal);
    glDeleteBuffers(1, &gBufferAlbedo);

    if (! softTerminate) {
        glDeleteVertexArrays(1, &quadVAO);
        gbufferSpriteShader.unload();
        pbrLightingShader.unload();
        if (debugRenderingEnabled) {
            debugShader.unload();
        }
    }
}

void DeferredRenderer::reset (float width, float height)
{
    term(true); // soft-terminate, don't delete things we're not recreating
    init(width, height, true); // soft-init, don't init things that are already inited
}

enum UberShaderMode {
    MODE_BACKGROUNDS = 0,
    MODE_BAKED_LIGHTING,
    MODE_3D_GEOMETRY,
    MODE_TILES,
    MODE_SPRITES
};

void DeferredRenderer::render (const Rect& screenBounds, const glm::mat4& view)
{
    glViewport(0, 0, screenWidth, screenHeight);

    // Load view into UBO
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    /// Render to g-buffer

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render solid stuff
    glDisable(GL_BLEND);

    // Render 3D geometry

    // Render solid objects

    gbufferSpriteShader.use();
    spritePool.render(screenBounds);
    checkErrors();

    // Render background images
    checkErrors();
//    gbufferBackgroundShader.use();
//    Shader::setUniform(u_texture, 5);
//    glBindVertexArray(quadVAO);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
//    checkErrors();
//    glBindVertexArray(0);

    // Render baked light and shodow maps

    /// Render g-buffer to framebuffer

    // Bind g-buffer
    pbrLightingShader.use();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBufferPosition);
    Shader::setUniform(pbrLightingShader.uniform("gPosition"), 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBufferNormal);
    Shader::setUniform(pbrLightingShader.uniform("gNormal"), 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBufferAlbedo);
    Shader::setUniform(pbrLightingShader.uniform("gAlbedoSpec"), 2);

    // TODO: set lights

    // Render fullscreen quad
    glBindVertexArray(quadVAO);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Copy depth buffer from gBuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /// Now render transparent items
    glEnable(GL_BLEND);

    // Render particles
//    particlesShader.use();

    // Render transparent objects
//    transparencyShader.use();

    // Render foreground objects

    if (debugRenderingEnabled) {
        /// Render debug information (render buffers to viewports)
        glEnable(GL_SCISSOR_TEST);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        debugShader.use();
        Shader::setUniform(u_debugTexture, 0);
        glBindVertexArray(quadVAO);
        checkErrors();

        // Render g-buffer
        int width = screenWidth / 8;
        int height = screenHeight / 8;
        int x = screenWidth - (width + 10);
        int y = screenHeight - (height + 10);
        for (auto mode : {0, 1}) {
            Shader::setUniform(u_debugMode, mode);
            for (auto buffer : {gBufferPosition, gBufferNormal, gBufferAlbedo}) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, buffer);
                glViewport(x, y, width, height);
                glScissor(x - 2, y - 2, width + 4, height + 4);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                y -= height + 10;
            }
        }

        glBindVertexArray(0);
        glDisable(GL_SCISSOR_TEST);
        glEnable(GL_DEPTH_TEST);
    }
}
