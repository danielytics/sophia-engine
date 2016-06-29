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
    GLuint projection () {return u_projection;}
    GLuint view () {return u_view;}

private:
    Mesh mesh;
    Shader_t spriteShader;
    GLuint tbo;
    GLuint tbo_tex;
    GLuint u_tbo_tex;
    GLuint u_texture;
    GLuint u_projection;
    GLuint u_view;

    unsigned spriteCount;
};

#endif // SPRITES_H
