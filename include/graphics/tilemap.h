#ifndef TILEMAP_H
#define TILEMAP_H

#include "shader.h"
#include "mesh.h"
#include <glm/glm.hpp>

class TileMap {
    using Shader_t = Shader::Shader;
public:
    ~TileMap ();

    void init (const std::vector<std::vector<float>>& map);
    void render (const glm::vec3& camera, const glm::mat4& projection, const glm::mat4& view);

private:
    Mesh mesh;
    Shader_t shader;
    GLuint u_projection;
    GLuint u_view;
    GLuint u_texture;
    int width;
    int height;
};

#endif // TILEMAP_H
