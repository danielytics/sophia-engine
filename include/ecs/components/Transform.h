#ifndef ECS_TRANSFORM_H
#define ECS_TRANSFORM_H

#include <glm/glm.hpp>

namespace ecs {

struct Transform {
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
};

}

#endif // ECS_TRANSFORM_H
