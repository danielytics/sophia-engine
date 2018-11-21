#ifndef TYPES_H
#define TYPES_H

#include <glm/glm.hpp>

struct Rect {
    // Coordinate system: top_left.x <= bottom_right.x, top_left.y >= bottom_right.y
    glm::vec2 top_left;
    glm::vec2 bottom_right;

    inline bool containes (const glm::vec2& point) const
    {
        return point.x >= top_left.x && point.x <= bottom_right.x &&
               point.y >= top_left.y && point.y <= bottom_right.y;
    }
};

struct Box {
    // Coordinate system: top_left.x <= bottom_right.x, top_left.y >= bottom_right.y, top_left.z >= bottom_right.z
    glm::vec3 top_left;
    glm::vec3 bottom_right;
};

#endif // TYPES_H
