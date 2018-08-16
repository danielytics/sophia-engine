#ifndef COMPONENT_H
#define COMPONENT_H


#include "util/Config.h"
#include "entt/entity/registry.hpp"
#include "entt/entity/prototype.hpp"
#include "lib.h"

namespace ecs::loader {

class ComponentCtor {
public:
    virtual ~ComponentCtor();
    virtual void construct (const entt::DefaultRegistry::entity_type& entity, const YAML::Node& config, entt::DefaultRegistry& registry) = 0;
    virtual void construct (entt::DefaultPrototype& prototype, const YAML::Node& config) = 0;
};

}

#endif // COMPONENT_H
