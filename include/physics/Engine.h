#ifndef ENGINE_H
#define ENGINE_H

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

private:
    struct PhysicsData* data;
};

}

#endif // ENGINE_H
