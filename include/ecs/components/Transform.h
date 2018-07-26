#ifndef ECS_TRANSFORM_H
#define ECS_TRANSFORM_H

#include <glm/glm.hpp>

namespace ecs {

struct Transform {
    glm::mat4 transformation;
};

}

#endif // ECS_TRANSFORM_H
