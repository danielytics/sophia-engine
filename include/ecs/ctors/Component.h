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
    virtual void construct (entt::DefaultPrototype& prototype, const YAML::Node& config) = 0;
};

template <typename Label>
class LabelCtor : public ComponentCtor {
public:
    void construct (entt::DefaultPrototype& prototype, const YAML::Node& config) {
        prototype.set<Label>();
    }
};

}

#endif // COMPONENT_H
