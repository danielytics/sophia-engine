#include "graphics/DeferredRenderer.h"
#include "util/Logging.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


DeferredRenderer::DeferredRenderer()
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
        gbufferShader = Shader::load("data/shaders/gbuffer.vert", "data/shaders/gbuffer.frag");
        pbrLightingShader = Shader::load("data/shaders/passthrough.vert", "data/shaders/pbr.frag");

        // Retrieve uniform locations
        u_gbuffer_rendermode = gbufferShader.uniform("renderMode");

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

        // Setup UBO for matrices
        glGenBuffers(1, &matrices_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
        glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, matrices_ubo, 0, 2 * sizeof(glm::mat4));
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
        gbufferShader.unload();
        pbrLightingShader.unload();
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
    // Load view into UBO
    glBindBuffer(GL_UNIFORM_BUFFER, matrices_ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Render solid stuff
    glDisable(GL_BLEND);
    gbufferShader.use();

    // Render 3D geometry
    Shader::setUniform(u_gbuffer_rendermode, MODE_3D_GEOMETRY);

    // Render solid objects
    Shader::setUniform(u_gbuffer_rendermode, MODE_TILES);

    Shader::setUniform(u_gbuffer_rendermode, MODE_SPRITES);

    // Render background images
    Shader::setUniform(u_gbuffer_rendermode, MODE_BACKGROUNDS);

    // Render baked light and shodow maps
    Shader::setUniform(u_gbuffer_rendermode, MODE_BAKED_LIGHTING);

    // Render g-buffer to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBufferPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gBufferNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBufferAlbedo);

    pbrLightingShader.use();
    // TODO: set lights

    // Render fullscreen quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    // Copy depth buffer from gBuffer
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Now render transparent stuff
    glEnable(GL_BLEND);

    // Render particles
//    particlesShader.use();

    // Render transparent objects
//    transparencyShader.use();

    // Render foreground objects
}
