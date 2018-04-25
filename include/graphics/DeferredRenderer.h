#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "Renderable.h"

class DeferredRenderer
{
public:
    explicit DeferredRenderer();
    ~DeferredRenderer();

    void init (float width, float height, bool softInitialise=false);
    void term (bool softTerminate=false);
    void reset (float width, float height); // Used to resize the window

    void render (const Rect& screenBounds, const glm::mat4& view);

    inline const glm::mat4& projection () const { return projection_matrix; }

private:
    Shader_t gbufferShader;
    Shader_t pbrLightingShader;
    Shader_t particlesShader;
    Shader_t transparencyShader;

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

    //
    unsigned int quadVAO;
    unsigned int quadVBO;
};

#endif // DEFERREDRENDERER_H
