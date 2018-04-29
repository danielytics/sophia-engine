#ifndef TYPES_H
#define TYPES_H

#include <glm/glm.hpp>

struct Rect {
    // Coordinate system: top_left.x <= bottom_right.x, top_left.y >= bottom_right.y
    glm::vec2 top_left;
    glm::vec2 bottom_right;
};

struct Box {
    // Coordinate system: top_left.x <= bottom_right.x, top_left.y >= bottom_right.y, top_left.z >= bottom_right.z
    glm::vec3 top_left;
    glm::vec3 bottom_right;
};

#endif // TYPES_H
