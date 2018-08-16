
#include "graphics/SpritePool.h"
#include "graphics/Debug.h"
#include "util/Logging.h"

using Shader_t = Shader::Shader;

SpritePool::SpritePool ()
    : visibleSprites(unsigned(-1))
{

}

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
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3), nullptr, GL_STREAM_DRAW); // This will get replaced on the first update
    glBindBuffer(GL_TEXTURE_BUFFER, 0);
    checkErrors();

    u_tbo_tex = spriteShader.uniform("u_tbo_tex");
    u_texture = spriteShader.uniform("u_texture");

    spriteCount = 0;
}

// TODO: avoid copying data by using some kind of quad tree or other spatially aware data structure. Also, set spriteCount as a static max on-screen limit (1k sprites?)
void SpritePool::update (const std::vector<Sprite>& sprites)
{
    if (spriteCount != sprites.size()) {
        if (spriteCount != 0) { // If this isn't the first update, then warn that the size has changed
            warn("SpritePool inited for {} sprites but {} sprites updated - this may have a performance impact", spriteCount, sprites.size());
        }
        spriteCount = unsigned(sprites.size());
        unsortedBuffer.reserve(spriteCount);
        sortedBuffer.reserve(spriteCount);
    }
    unsortedBuffer.clear();
    std::copy(sprites.begin(), sprites.end(), std::back_inserter(unsortedBuffer));
}

void SpritePool::render (const Rect& bounds)
{
    glm::vec2 centerPoint = (bounds.top_left + bounds.bottom_right) * 0.5f;
    if (visibleSprites == unsigned(-1) || centerPoint != prevCenterPoint) {
        prevCenterPoint = centerPoint;
        unsigned index = 0;
        float screenRadius = glm::distance(centerPoint, glm::vec2(bounds.top_left));
        for (const Sprite& sprite : unsortedBuffer) {
            float spriteRadius = 1.2f; // TODO: This should be part of the sprite data (scale factor?)
            if (glm::distance(sprite.position, centerPoint) <= screenRadius + spriteRadius) {
                sortedBuffer[index] = sprite;
                ++index;
            }
        }
        visibleSprites = index;

        glActiveTexture(GL_TEXTURE0 + 6);
        glBindBuffer(GL_TEXTURE_BUFFER, tbo);
        glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
        // Orphan old buffer and then load data into new buffer
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteCount, nullptr, GL_STREAM_DRAW);
        glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteCount, reinterpret_cast<const float*>(sortedBuffer.data()), GL_STREAM_DRAW);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);
        glBindBuffer(GL_TEXTURE_BUFFER, 0);
    }

    trace("Rendering {} visible sprites ({} total)", visibleSprites, spriteCount);

    Shader::setUniform(u_texture, 5);
    Shader::setUniform(u_tbo_tex, 6);
    checkErrors();
    mesh.draw(visibleSprites);
}
