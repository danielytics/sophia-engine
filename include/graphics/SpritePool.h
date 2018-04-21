#ifndef SPRITES_H
#define SPRITES_H

#include "renderable.h"
#include "mesh.h"

struct Sprite {
    glm::vec2 position;
    float image;
};

class SpritePool : public Renderable {
public:
    ~SpritePool ();

    void init (unsigned numSprites);
    void update (const std::vector<Sprite>& sprites);

    void render (const Rect& bounds);
    Shader_t shader () {return spriteShader;}
    Uniform_t projection () {return u_projection;}
    Uniform_t view () {return u_view;}

private:
    Mesh mesh;
    Shader_t spriteShader;
    Buffer_t tbo;
    Buffer_t tbo_tex;
    Uniform_t u_tbo_tex;
    Uniform_t u_texture;
    Uniform_t u_projection;
    Uniform_t u_view;

    unsigned spriteCount;
};

#endif // SPRITES_H
