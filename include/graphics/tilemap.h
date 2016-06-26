#ifndef TILEMAP_H
#define TILEMAP_H

#include "renderable.h"
#include "mesh.h"

class TileMap : public Renderable {
public:
    ~TileMap ();

    void init (const std::vector<std::vector<float>>& map);
    void reset (const std::vector<std::vector<float>>& map);

    void render (const Rect& bounds);
    Shader_t shader () {return tileShader;}
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
