#ifndef ENGINE_H
#define ENGINE_H

#include <glm/glm.hpp>
#include "util/Config.h"

namespace physics {

class Engine
{
public:
    // Init
    Engine ();

    void init (const YAML::Node& config_node);

    // Term
    ~Engine ();

    void step (float dt);

    void addBody (const glm::vec2& position, const glm::vec2& halfExtents);

private:
    struct PhysicsData* data;
};

}

#endif // ENGINE_H
