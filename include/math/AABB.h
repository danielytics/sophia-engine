#ifndef AABB_H
#define AABB_H

#include "Types.h"

class AABB {
public:
    AABB(const glm::vec2& top_left, const glm::vec2& bottom_right) : rect({top_left, bottom_right}) {}
    AABB(const Rect& rect) : rect(rect) {}
    AABB(const Rect&& rect) : rect(std::move(rect)) {}

    inline glm::vec2 centerPoint () {
        return glm::vec2((rect.top_left.x + rect.bottom_right.x) * 0.5f, (rect.top_left.y + rect.bottom_right.y) * 0.5f);
    }

    inline bool contains (const AABB& other) const {
        return other.rect.top_left.x >= rect.top_left.x && other.rect.top_left.x < rect.bottom_right.x &&
               other.rect.bottom_right.x >= rect.top_left.x && other.rect.bottom_right.x < rect.bottom_right.x &&
               other.rect.top_left.y <= rect.top_left.y && other.rect.top_left.y > rect.bottom_right.y &&
               other.rect.bottom_right.y <= rect.top_left.y && other.rect.bottom_right.y > rect.bottom_right.y;
    }

    inline bool overlaps (const AABB& other) const {
        return ((other.rect.top_left.x >= rect.top_left.x && other.rect.top_left.x < rect.bottom_right.x) ||
                (other.rect.bottom_right.x >= rect.top_left.x && other.rect.bottom_right.x < rect.bottom_right.x)) &&
               ((other.rect.top_left.y <= rect.top_left.y && other.rect.top_left.y > rect.bottom_right.y) ||
                (other.rect.bottom_right.y <= rect.top_left.y && other.rect.bottom_right.y > rect.bottom_right.y));
    }

    inline AABB grow (const AABB& other) const {
        Rect r = {
            {glm::min(rect.top_left.x, other.rect.top_left.x), glm::max(rect.top_left.y, other.rect.top_left.y)},
            {glm::max(rect.bottom_right.x, other.rect.bottom_right.x), glm::min(rect.bottom_right.y, other.rect.bottom_right.y)},
        };
        return AABB(std::move(r));
    }

    // Coordinate system: top_left.x <= bottom_right.x, top_left.y >= bottom_right.y
    const Rect rect;
};

#endif // AABB_H
