#ifndef ECS_HIERARCHY_H
#define ECS_HIERARCHY_H

#include "lib.h"
#include "entt/entity/registry.hpp"

namespace ecs {

struct Parent
{
    entt::DefaultRegistry::entity_type parent;
};

struct Children {
    lib::vector<entt::DefaultRegistry::entity_type> children;
};

}

#endif // ECS_HIERARCHY_H
