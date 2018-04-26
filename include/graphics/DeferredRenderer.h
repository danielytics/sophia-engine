#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "Renderable.h"
#include "SpritePool.h"

class DeferredRenderer
{
public:
    explicit DeferredRenderer(bool enabledDebugRendering=false);
    ~DeferredRenderer();

    void init (float width, float height, bool softInitialise=false);
    void term (bool softTerminate=false);
    void reset (float width, float height); // Used to resize the window

    void render (const Rect& screenBounds, const glm::mat4& view);

    inline const glm::mat4& projection () const { return projection_matrix; }

    // Renderables
    inline void updateSprites (const std::vector<Sprite>& sprites) {
        spritePool.update(sprites);
    }

private:
    bool debugRenderingEnabled;

    Shader_t gbufferBackgroundShader;
    Uniform_t u_texture;

    Shader_t gbufferSpriteShader;
    Shader_t pbrLightingShader;
    Shader_t particlesShader;
    Shader_t transparencyShader;

    Shader_t debugShader;
    Uniform_t u_debugTexture;
    Uniform_t u_debugMode;

    GLsizei screenWidth;
    GLsizei screenHeight;

    glm::mat4 projection_matrix;

    Buffer_t matrices_ubo;

    /// Data for deferred shading
    // Framebuffer object and depth buffer
    Buffer_t gBuffer;
    Buffer_t gBufferDepth;
    // Render Targets
    Buffer_t gBufferPosition, gBufferNormal, gBufferAlbedo;
    // Uniforms
    Uniform_t u_gbuffer_rendermode;

    // Used to render fullscreen quads
    Buffer_t quadVAO;
    Buffer_t quadVBO;

    // Renderables
    SpritePool spritePool;
};

#endif // DEFERREDRENDERER_H
