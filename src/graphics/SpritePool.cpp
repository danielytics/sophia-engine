
#include "graphics/spritepool.h"
#include "util/logging.h"

using Shader_t = Shader::Shader;

SpritePool::~SpritePool () {
    spriteShader.unload();
}

void SpritePool::init (unsigned numSprites)
{
    spriteShader = Shader::load("data/shaders/sprites.vert", "data/shaders/sprites.frag");
    mesh = Mesh{};
    mesh.addBuffer(std::vector<glm::vec3>{
            {-0.5f, 0.5f, 0.5f},
            { 0.5f, 0.5f, 0.5f},
            { 0.5f,-0.5f, 0.5f},
            { 0.5f,-0.5f, 0.5f},
            {-0.5f,-0.5f, 0.5f},
            {-0.5f, 0.5f, 0.5f},
        }, true);
    mesh.addBuffer(std::vector<glm::vec2>{
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},
        });

    glGenBuffers(1, &tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glGenTextures(1, &tbo_tex);
    glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * numSprites, 0, GL_STREAM_DRAW);

    u_tbo_tex = glGetUniformLocation(spriteShader.programID, "u_tbo_tex");
    u_projection = glGetUniformLocation(spriteShader.programID, "u_projection");
    u_view = glGetUniformLocation(spriteShader.programID, "u_view");
    u_texture = glGetUniformLocation(spriteShader.programID, "u_texture");

    spriteCount = numSprites;
}

void SpritePool::update (const std::vector<Sprite>& sprites)
{
    if (spriteCount != sprites.size()) {
        warn("SpritePool inited for {} sprites but {} sprites updated", spriteCount, sprites.size());
        spriteCount = sprites.size();
    }
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
    // Orphan old buffer and then load data into new buffer
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteCount, 0, GL_STREAM_DRAW);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteCount, reinterpret_cast<const float*>(sprites.data()), GL_STREAM_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);
}

void SpritePool::render (const Rect& bounds)
{
    trace("Rendering {} sprites", spriteCount);
    glUniform1i(u_texture, 0);
    glUniform1i(u_tbo_tex, 1);
    mesh.draw(spriteCount);
}
