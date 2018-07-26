
#include "ecs/Loader.h"
#include "entt/entt.hpp"
#include "util/Config.h"
#include "util/Logging.h"

#include "ecs/components/Hierarchy.h"
#include "ecs/components/TimeAware.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace ecs::loader;

#include "ecs/components/Transform.h"

ComponentCtor::~ComponentCtor() {}

class LocationComponentCtor : public ComponentCtor {
public:
    void construct (const entt::DefaultRegistry::entity_type& entity, const YAML::Node& config, entt::DefaultRegistry& registry);
};

void LocationComponentCtor::construct (const entt::DefaultRegistry::entity_type& entity, const YAML::Node& config, entt::DefaultRegistry& registry) {
    glm::vec3 translation;
    glm::vec3 rotation;
    auto parser = Config::make_parser(
                Config::optional(
                    Config::map("position",
                        Config::scalar("x", translation.x),
                        Config::scalar("y", translation.y),
                        Config::scalar("z", translation.z)),
                    Config::map("rotation",
                        Config::scalar("x", rotation.x),
                        Config::scalar("y", rotation.y),
                        Config::scalar("z", rotation.z))
                )
    );
    parser(config);

    // Create transformation matrix
    glm::mat4 transform(1.0);
    transform = glm::rotate(transform, glm::radians(rotation.x * 360.0f), glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, glm::radians(rotation.y * 360.0f), glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, glm::radians(rotation.z * 360.0f), glm::vec3(0, 0, 1));
    transform = glm::translate(transform, translation);
    // Assign transformation component with transformation matrix to entity
    registry.assign<ecs::Transform>(entity, transform);
}


std::map<std::string, ComponentCtor*> EntityLoader::constructors {
    {"transform", new LocationComponentCtor},
};

void EntityLoader::loadScene (const YAML::Node& sceneConfig)
{
    std::map<std::string, entt::DefaultRegistry::entity_type> entities;
    entt::DefaultRegistry registry;

    auto parser = Config::make_parser(
                Config::fn("scene", [&registry, this](const YAML::Node scene) {
                    loadScene(scene, registry);
                    return Config::Success;
                })
    );
    parser(sceneConfig);
}

std::vector<entt::DefaultRegistry::entity_type> EntityLoader::loadScene (YAML::Node scene, entt::DefaultRegistry& registry)
{
    std::vector<entt::DefaultRegistry::entity_type> entity_ids;
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
                            auto group = loadGroup(nodeName, node, registry);
                            entity_ids.insert(entity_ids.end(), group.begin(), group.end());
                        } else if (type == "entity") {
                            entity_ids.push_back(loadEntity(nodeName, node, registry));
                        } else if (type == "template") {

                        }
                    }
                }
            }
        }
    }
    return entity_ids;
}

entt::DefaultRegistry::entity_type EntityLoader::loadEntity (const std::string& entityName, const YAML::Node& entityConfig, entt::DefaultRegistry& registry)
{
    auto entity = registry.create();
    entities.emplace(entityName, entity);

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
                }
            }
        }
    }

    // Add TimeAware, if not manually added
    if (! registry.has<TimeAware>(entity)) {
        registry.assign<TimeAware>(1.0f);
    }

    // Load children scene and setup parent-child hierarchy
    auto children_node = entityConfig["children"];
    if (children_node.IsMap()) {
        auto children = loadScene(children_node, registry);
        for (auto child : children) {
            registry.assign<ecs::Parent>(child, Parent{entity});
        }
        registry.assign<ecs::Children>(entity, Children{children});
    }

    return entity;
}

std::vector<entt::DefaultRegistry::entity_type> EntityLoader::loadGroup (const std::string& groupName, const YAML::Node& groupConfig, entt::DefaultRegistry& registry)
{
#ifdef DEBUG_BUILD
    auto group_comment = groupConfig["comment"];
    if (group_comment.IsScalar()) {
        debug("Loading group: {} - {}", groupName, group_comment.as<std::string>());
    } else {
        debug("Loading group: {}", groupName);
    }
#endif

    std::vector<entt::DefaultRegistry::entity_type> group;
    auto children_node = groupConfig["children"];
    if (children_node.IsMap()) {
        // Load group scene
        group = loadScene(children_node, registry);
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
                        for (auto entity : group) {
                            iter->second->construct(entity, component_data, registry);
                        }
                    }
                }
            }
        }
    }
    return group;
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
