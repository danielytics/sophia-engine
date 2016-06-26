#ifndef TILEMAP_H
#define TILEMAP_H

#include "shader.h"
#include "mesh.h"
#include <glm/glm.hpp>

struct Rect {
    glm::vec3 top_left;
    glm::vec3 bottom_right;
};

using Shader_t = Shader::Shader;

class Renderable {
public:
    virtual Shader_t shader ()=0;
    virtual void render (const Rect& bounds)=0;
    virtual GLuint projection ()=0;
    virtual GLuint view ()=0;
};

class TileMap : public Renderable {
public:
    ~TileMap ();

    void init (const std::vector<std::vector<float>>& map);
    void reset (const std::vector<std::vector<float>>& map);
    Shader_t shader () {return tileShader;}
    void render (const Rect& bounds);

    GLuint projection () {return u_projection;}
    GLuint view () {return u_view;}
private:
    Mesh mesh;
    Shader_t tileShader;
    GLuint u_texture;
    GLuint u_projection;
    GLuint u_view;
    unsigned imageIdBuffer;
    int width;
    int height;
    int max_index;
};

#endif // TILEMAP_H
