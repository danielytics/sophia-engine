#ifndef MESH_H
#define MESH_H

#include "lib.h"
#include <glm/glm.hpp>

namespace ecs {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Mesh
{
    lib::vector<Vertex> vertices;
    lib::vector<unsigned int> indices;
};

}

#endif // MESH_H
