#ifndef LOADER_H
#define LOADER_H

#include "lib.h"
#include <string>

#include "ctors/Component.h"

namespace ecs::loader {

struct EntityBlueprint {
    entt::DefaultPrototype prototype;
    lib::vector<EntityBlueprint> children;
};

using entity_t = entt::DefaultRegistry::entity_type;

class EntityLoader {
public:
    EntityLoader (entt::DefaultRegistry& registry);
    ~EntityLoader ();

    void load(const YAML::Node& config);

    void loadScene (const std::string& sceneFile);
    lib::vector<EntityBlueprint> loadScene (const YAML::Node& scene);
    lib::vector<EntityBlueprint> loadGroup (const std::string& groupName, const YAML::Node& groupConfig);
    EntityBlueprint loadEntity (const std::string& entityName, const YAML::Node& entityConfig);
    EntityBlueprint loadTemplate (const std::string& templateName, const YAML::Node& entityConfig);

    entity_t instantiate (const EntityBlueprint& blueprint);

private:
    EntityBlueprint loadTemplate (const std::string& templateSourceFile, const std::string& templateName);

    entt::DefaultRegistry& registry;
    lib::map<std::string, entt::DefaultRegistry::entity_type> entities;

    static lib::map<std::string, ComponentCtor*> constructors;
};

}

#endif // LOADER_H
