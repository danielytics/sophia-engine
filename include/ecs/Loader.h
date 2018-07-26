#ifndef LOADER_H
#define LOADER_H

#include "util/Config.h"
#include "entt/entity/registry.hpp"

/*

root:
  type: group
  children:
    - level

*/

namespace ecs::loader {

class ComponentCtor {
public:
    virtual ~ComponentCtor();
    virtual void construct (const entt::DefaultRegistry::entity_type& entity, const YAML::Node& config, entt::DefaultRegistry& registry) = 0;
};

class EntityLoader {
public:
    void loadScene (const YAML::Node& sceneConfig);
    std::vector<entt::DefaultRegistry::entity_type> loadScene (YAML::Node scene, entt::DefaultRegistry& registry);
    std::vector<entt::DefaultRegistry::entity_type> loadGroup (const std::string& groupName, const YAML::Node& groupConfig, entt::DefaultRegistry& registry);
    entt::DefaultRegistry::entity_type loadEntity (const std::string& entityName, const YAML::Node& entityConfig, entt::DefaultRegistry& registry);

private:
    std::map<std::string, entt::DefaultRegistry::entity_type> entities;

    static std::map<std::string, ComponentCtor*> constructors;
};

}

#endif // LOADER_H
