#ifndef DEFERREDRENDERER_H
#define DEFERREDRENDERER_H

#include "renderable.h"

class DeferredRenderer
{
public:
    explicit DeferredRenderer();
    ~DeferredRenderer();

    void init ();
    void render ();

private:
    Shader_t gbufferShader;
    Shader_t pbrLightingShader;
    Shader_t particlesShader;
    Shader_t transparencyShader;

    GLsizei screenWidth;
    GLsizei screenHeight;

    // FBO
    Buffer_t gBuffer;
    Buffer_t gBufferDepth;
    // RT
    Buffer_t gBufferPosition, gBufferNormal, gBufferAlbedo;

    //
    unsigned int quadVAO;
    unsigned int quadVBO;
};

#endif // DEFERREDRENDERER_H
