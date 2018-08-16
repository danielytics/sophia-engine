#include "ecs/Loader.h"

#include "util/Config.h"
#include "util/Logging.h"
#include "util/Helpers.h"

#include "entt/entt.hpp"

#include "ecs/components/Hierarchy.h"
#include "ecs/components/TimeAware.h"

#include "ecs/ctors/Transform.h"

using namespace ecs::loader;

ComponentCtor::~ComponentCtor() {}

lib::map<std::string, ComponentCtor*> EntityLoader::constructors {
    {"transform", new TransformComponentCtor},
};

EntityLoader::EntityLoader (entt::DefaultRegistry& registry)
    : registry(registry)
{

}

EntityLoader::~EntityLoader ()
{

}

void EntityLoader::loadScene (const std::string& sceneFile)
{
    auto parser = Config::make_parser(
                Config::fn("scene", [this](const YAML::Node scene) {
                    for (auto& blueprint : loadScene(scene)) {
                        instantiate(blueprint);
                    }
                    return Config::Success;
                })
    );
    parser(YAML::Load(Helpers::readToString(sceneFile)));
}

lib::vector<EntityBlueprint> EntityLoader::loadScene (YAML::Node scene)
{
    lib::vector<EntityBlueprint> blueprints;
    if (scene.IsMap()) {
        for (auto it = scene.begin(); it != scene.end(); ++it) {
            auto name = it->first;
            auto node = it->second;
            if (name.IsScalar()) {
                auto nodeName = name.as<std::string>();
                if (node.IsMap()) {
                    auto node_type = node["type"];
                    if (node_type.IsScalar()) {
                        auto type = node_type.as<std::string>();
                        if (type == "group") {
                            auto group = loadGroup(nodeName, node);
                            Helpers::move_back(group, blueprints);
                        } else if (type == "entity") {
                            blueprints.push_back(loadEntity(nodeName, node));
                        } else if (type == "template") {
                            blueprints.push_back(loadTemplate(nodeName, node));
                        }
                    }
                }
            }
        }
    }
    return blueprints;
}

EntityBlueprint EntityLoader::loadEntity (const std::string& entityName, const YAML::Node& entityConfig)
{
    auto entity = registry.create();
    entities.emplace(entityName, entity);
    auto prototype = entt::DefaultPrototype(registry);

#ifdef DEBUG_BUILD
    auto component_comment = entityConfig["comment"];
    if (component_comment.IsScalar()) {
        debug("Loading entity: {} - {}", entityName, component_comment.as<std::string>());
    } else {
        debug("Loading entity: {}", entityName);
    }
#endif

    // Add components to entity
    auto components_node = entityConfig["components"];
    if (components_node.IsMap()) {
        for (auto it = components_node.begin(); it != components_node.end(); ++it) {
            auto component_name = it->first;
            auto component_data = it->second;
            if (component_name.IsScalar()) {
                auto iter = constructors.find(component_name.as<std::string>());
                if (iter != constructors.end()) {
                    // Construct new component and add it to entity
                    iter->second->construct(entity, component_data, registry);
                    iter->second->construct(prototype, component_data);
                }
            }
        }
    }

    // Add child scenes and setup parent-child hierarchy
    lib::vector<EntityBlueprint> child_entities;
    auto children_node = entityConfig["children"];
    if (children_node.IsMap()) {
        auto children = loadScene(children_node);
        Helpers::move_back(children, child_entities);
    }

    return EntityBlueprint{lib::move(prototype), lib::move(child_entities)};
}

lib::vector<EntityBlueprint> EntityLoader::loadGroup (const std::string& groupName, const YAML::Node& groupConfig)
{
#ifdef DEBUG_BUILD
    auto group_comment = groupConfig["comment"];
    if (group_comment.IsScalar()) {
        debug("Loading group: {} - {}", groupName, group_comment.as<std::string>());
    } else {
        debug("Loading group: {}", groupName);
    }
#endif

    lib::vector<EntityBlueprint> group;
    auto children_node = groupConfig["children"];
    if (children_node.IsMap()) {
        // Load group scene
        group = loadScene(children_node);
        // Add default components to group members
        auto defaults_node = groupConfig["defaults"];
        if (defaults_node.IsMap()) {
            for (auto it = defaults_node.begin(); it != defaults_node.end(); ++it) {
                auto component_name = it->first;
                auto component_data = it->second;
                if (component_name.IsScalar()) {
                    auto iter = constructors.find(component_name.as<std::string>());
                    if (iter != constructors.end()) {
                        // Construct new component and add it to the entities in the group
                        for (auto& blueprint : group) {
                            iter->second->construct(blueprint.prototype, component_data);
                        }
                    }
                }
            }
        }
    }
    return group;
}

EntityBlueprint EntityLoader::loadTemplate (const std::string& templateName, const YAML::Node& templateConfig)
{
    auto source_node = templateConfig["source"];
    if (source_node.IsScalar()) {
        auto source = source_node.as<std::string>();
#ifdef DEBUG_BUILD
        auto group_comment = templateConfig["comment"];
        if (group_comment.IsScalar()) {
            debug("Loading template: {}:{} - {}", templateName, source, group_comment.as<std::string>());
        } else {
            debug("Loading template: {}:{}", templateName, source);
        }
#endif

        // Load entity from template source file.
        auto blueprint = loadTemplate(source, templateName);

        // Add children, if any
        auto children_node = templateConfig["children"];
        if (children_node.IsMap()) {
            auto children = loadScene(children_node);
            Helpers::move_back(children, blueprint.children);
        }

        // TODO: Instantiate instances

        return blueprint;
    } else {
        warn("Error loading template {}: No source specified", templateName);
        return EntityBlueprint{lib::move(entt::DefaultPrototype{registry}), lib::vector<EntityBlueprint>{}}; // throw instead?
    }
}

EntityBlueprint EntityLoader::loadTemplate (const std::string& templateSourceFile, const std::string& templateName)
{
    lib::vector<EntityBlueprint> blueprints;
    auto parser = Config::make_parser(
                Config::fn("template", [this,templateName,&blueprints](const YAML::Node node) {
                    if (node.IsMap()) {
                        auto node_type = node["type"];
                        if (node_type.IsScalar()) {
                            auto type = node_type.as<std::string>();
                            if (type == "entity") {
                                blueprints.push_back(loadEntity(templateName, node));
                            } else if (type == "template") {
                                blueprints.push_back(loadTemplate(templateName, node));
                            } else {
                                warn("Invalid template type: {}", type);
                            }
                        }
                    }
                    return Config::Success;
                })
    );
    parser(YAML::Load(Helpers::readToString(templateSourceFile)));
    if (blueprints.size() != 0) {
        error("Failed to load template {} from file: {}", templateName, templateSourceFile);
        return EntityBlueprint{lib::move(entt::DefaultPrototype{registry}), lib::vector<EntityBlueprint>{}}; // throw insteadm?
    } else {
        return EntityBlueprint{lib::move(blueprints[0].prototype), lib::move(blueprints[0].children)};
    }
}

entity_t EntityLoader::instantiate (const EntityBlueprint& blueprint)
{
    entity_t entity = blueprint.prototype.create();
    if (! registry.has<TimeAware>(entity)) {
        registry.assign<TimeAware>(1.0f);
    }
    lib::vector<entity_t> children;
    for (auto& child_blueprint : blueprint.children) {
        entity_t child = instantiate(child_blueprint); // recursively create child entitiies
        registry.assign<ecs::Parent>(child, Parent{entity});
        children.push_back(child);
    }
    registry.assign<ecs::Children>(entity, Children{children});
    return entity;
}

#if 0
#include "entt/entt.hpp"
#include <utility>
#include <experimental/optional>

using entity_t = entt::DefaultRegistry::entity_type;

class Scene {
public:
    template <typename S> struct Ctrl {
        S state;
        bool stop;
    };
    template <typename S> static Ctrl<S> advance(S state) {
        return Ctrl<S>{state, false};
    }
    template <typename S> static Ctrl<S> halt() {
        return Ctrl<S>{S{}, true};
    }

    entity_t getParentEntity (entity_t id);
    std::vector<entity_t> getChildEntities (entity_t id);

    /**
     *  Run an update funciton recursively over all entities with component C starting at entity with id `root`.
     *  This will look at all descendent entities of `root`, searching for all descendents with component C.
     */
    template <typename C, typename S>
    void updateTreeDeep (entity_t root, S rootState, std::function<Ctrl<S> (const S&, C&)> fn) {
        std::vector<std::pair<entity_t, S>> children;
        for (auto entity_id : getChildEntities(root)) {
            children.push_back(std::make_pair(entity_id, rootState));
        }
        while (! children.empty()) {
            auto [entity_id, state] = children.back();
            children.pop_back();
            // If the entity has the component, run the update function on it
            if (registry->has<C>(entity_id)) {
                auto [s, stop] = fn(state, registry->get<C>(entity_id));
                if (stop) {
                    continue;
                }
                state = s;
            }
            // Queue all child entities of current node
            for (auto child : getChildEntities(entity_id)) {
                children.push_back(std::make_pair(child, state));
            }
        }
    }

    /**
     *  Run an update funciton recursively over entities with component C starting at entity with id `root`, stopping at any branch without component C.
     *  This will look at all child entities of `root`, searching recursively for all descendent children with component C.
     *  Unlike updateTreeDeep, which finds all entities with component C even if they are not direct descendents, updateTreeShallow only searches as
     *  long as the entities have component C and will stop at any children that don't have it (even if deeper descendents have it).
     */
    template <typename C, typename S>
    void updateTreeShallow (entity_t root, S rootState, std::function<Ctrl<S> (const S&, C&)> fn) {
        std::vector<std::pair<entity_t, S>> children;
        for (auto entity_id : getChildEntities(root)) {
            if (registry->has<C>(entity_id)) {
                children.push_back(std::make_pair(entity_id, rootState));
            }
        }
        while (! children.empty()) {
            auto [entity_id, state] = children.back();
            children.pop_back();
            // Run the update function on entity
            auto [new_state, stop] = fn(state, registry->get<C>(entity_id));
            if (!stop) {
                // Queue child entities of current entity only if they have the component
                for (auto child : getChildEntities(entity_id)) {
                    if (registry->has<C>(entity_id)) {
                        children.push_back(std::make_pair(child, new_state));
                    }
                }
            }
        }
    }

private:
    entt::DefaultRegistry* registry;
};

enum EventHandlerResult {
    ConsumeEvent,
    PropogateEvent
};

struct Behavior {
    std::bitset<25> event_types;
    std::function<EventHandlerResult (unsigned)> handler;
};

struct Transform {
};

void test () {
    Scene scene;

    entity_t eventEmitterId = 0;
    struct EventState {
        unsigned event_type;
    };
    scene.updateTreeDeep<Behavior, EventState>(eventEmitterId, EventState{}, [](const auto& state, auto& behavior){
        if (behavior.event_types.test(state.event_type)) {
            if (behavior.handler(state.event_type) == ConsumeEvent) {
                return Scene::halt<EventState>();
            }
        }
        return Scene::advance(state);
    });

    struct TransformState {
    };
    scene.updateTreeDeep<Transform, TransformState>(eventEmitterId, TransformState{}, [](const auto& state, auto& transform){
        // Transform 'transform' by parent transform in state
        // Set state to 'transform' so its children can be set relatively
        return Scene::advance(state);
    });
}


struct Location {};
struct Sprite {};
struct RigidBody {};

namespace systems {

struct Render {
    static void update(Location& location, Sprite& sprite) {

    }
};

struct Audio {
    static void update(Location& location) {

    }
};

struct Animation {
    static void update(Sprite& sprite) {

    }
};

struct Physics {
    static void update(Location& location, RigidBody& rigidBody) {

    }
};

}

template <typename... Systems>
struct SystemUpdater {
    template <typename... Args> static void init(Args&...) {

    }
    template <typename... Args> static void update(Args&...) {

    }
    template <typename... Args> static void term(Args&...) {

    }
};

template <typename System, typename... Systems>
struct SystemUpdater<System, Systems...> {
    template <typename... Args> static void init(Args&... args) {
        System::init(args...);
        SystemUpdater<Systems...>::init(args...);
    }
    template <typename... Args> static void update(Args&... args) {
        System::update(args...);
        SystemUpdater<Systems...>::update(args...);
    }
    template <typename... Args> static void term(Args&... args) {
        System::term(args...);
        SystemUpdater<Systems...>::term(args...);
    }
};

#define new_task

struct TaskData {
    std::atomic_int counter;
};

template <typename System>
struct WrapTask {
     static void update(TaskData& data, entt::DefaultRegistry &registry) {
         data.counter++;
         new_task {
             System::update(registry);
         }
     }
};

template <typename... Systems>
struct Parallel {
    static void init() {
        // init sequentially
        SystemUpdater<Systems...>::init();
    }

    static void update(entt::DefaultRegistry &registry) {
        // update in parallel by spawing tasks
        TaskData data;
        SystemUpdater<WrapTask<Systems>...>::update(data, registry);
        // wait until data.counter == 0
    }

    static void term() {
        // terminate sequentially
        SystemUpdater<Systems...>::term();
    }
};

template <typename... S>
class SystemsSpec {
public:
    static void init() {
        SystemUpdater<S...>::init();
    }
    static void update(entt::DefaultRegistry &registry) {
        SystemUpdater<S...>::update(registry);
    }
    static void term() {
        SystemUpdater<S...>::term();
    }
};

template <typename T>
struct Getter {
    template <typename V, typename E> static T& get (V& view, E entity) {
        return view.get(entity);
    }
};

template <typename Sys, typename... Components>
class StatelessSystem {
public:
    static void init() {
    }
    static void update(entt::DefaultRegistry &registry) {
        auto view = registry.view<Components...>();
        view.each(Sys::update);
    }
    static void term() {
    }
};

template <typename Sys, typename... Components>
class BatchedStatelessSystem {
public:
    static void init() {
    }
    static void update(entt::DefaultRegistry &registry) {
        auto view = registry.view<Components...>();
        view.each(Sys::update);
    }
    static void term() {
    }
};

template <typename Sys, typename... Components>
class StatefulSystem {
public:
    static void init() {
    }
    static void update(entt::DefaultRegistry &registry) {
        auto view = registry.view<Components...>();
        view.each(Sys::update);
    }
    static void term() {
    }
};

using RenderSystem = StatelessSystem<systems::Render, Location, Sprite>;
using AudioSystem = StatelessSystem<systems::Audio, Location>;
using AnimationSystem = StatelessSystem<systems::Animation, Sprite>;
using PhysicsSystem = StatelessSystem<systems::Physics, Location, RigidBody>;

using Systems = SystemsSpec<
    Parallel<RenderSystem, AudioSystem>,
    AnimationSystem,
    PhysicsSystem>;

void foo () {
    Systems::init();

    entt::DefaultRegistry registry;
    Systems::update(registry);

    Systems::term();
}
#endif
