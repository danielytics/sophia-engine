#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Component.h"

class TransformComponentCtor : public ecs::loader::ComponentCtor {
public:
    void construct (const entt::DefaultRegistry::entity_type& entity, const YAML::Node& config, entt::DefaultRegistry& registry);
    void construct (entt::DefaultPrototype& prototype, const YAML::Node& config);
};

#endif // TRANSFORM_H
