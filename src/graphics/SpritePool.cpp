
#include "graphics/SpritePool.h"
#include "graphics/Debug.h"
#include "util/Logging.h"

using Shader_t = Shader::Shader;

SpritePool::~SpritePool () {
}

void SpritePool::init (const Shader_t& spriteShader)
{
    mesh.bind();
    mesh.addBuffer(std::vector<glm::vec3>{
            {-1.0f,  1.0f, 0.0f},
            {-1.0f, -1.0f, 0.0f},
            {1.0f,  1.0f, 0.0f},
            {1.0f, -1.0f, 0.0f}
        }, true);
    mesh.addBuffer(std::vector<glm::vec2>{
            {0.0f, 1.0f},
            {0.0f, 0.0f},
            {1.0f, 1.0f},
            {1.0f, 0.0f}
        });

    glGenBuffers(1, &tbo);
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glGenTextures(1, &tbo_tex);
    glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3), 0, GL_STREAM_DRAW); // This will get replaced on the first update
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    checkErrors();

    u_tbo_tex = spriteShader.uniform("u_tbo_tex");
    u_texture = spriteShader.uniform("u_texture");

    spriteCount = 0;
}

void SpritePool::update (const std::vector<Sprite>& sprites)
{
    if (spriteCount != sprites.size()) {
        if (spriteCount != 0) { // If this isn't the first update, then warn that the size has changed
            warn("SpritePool inited for {} sprites but {} sprites updated", spriteCount, sprites.size());
        }
        spriteCount = unsigned(sprites.size());
    }
    glActiveTexture(GL_TEXTURE0 + 6);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
    // Orphan old buffer and then load data into new buffer
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteCount, 0, GL_STREAM_DRAW);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteCount, reinterpret_cast<const float*>(sprites.data()), GL_STREAM_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    checkErrors();
}

void SpritePool::render (const Rect& bounds)
{
    trace("Rendering {} sprites", spriteCount);
    Shader::setUniform(u_texture, 5);
    Shader::setUniform(u_tbo_tex, 6);
    checkErrors();
    mesh.draw(spriteCount);
}
