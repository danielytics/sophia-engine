#ifndef SPRITES_H
#define SPRITES_H

#include "Renderable.h"
#include "Mesh.h"

struct Sprite {
    glm::vec2 position;
    float image;
};

class SpritePool : public Renderable {
public:
    ~SpritePool ();

    void init (const Shader_t& spriteShader);
    void update (const std::vector<Sprite>& sprites);

    void render (const Rect& bounds);

private:
    Mesh mesh;
    Buffer_t tbo;
    Buffer_t tbo_tex;
    Uniform_t u_tbo_tex;
    Uniform_t u_texture;

    unsigned spriteCount;
};

#endif // SPRITES_H
