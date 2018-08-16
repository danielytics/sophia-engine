#include "ecs/ctors/Transform.h"
#include "ecs/components/Transform.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

void TransformComponentCtor::construct (const entt::DefaultRegistry::entity_type& entity, const YAML::Node& config, entt::DefaultRegistry& registry) {
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
    auto parser = Config::make_parser(
                Config::optional(
                    Config::map("position",
                        Config::scalar("x", translation.x),
                        Config::scalar("y", translation.y),
                        Config::scalar("z", translation.z)),
                    Config::map("rotation",
                        Config::scalar("x", rotation.x),
                        Config::scalar("y", rotation.y),
                        Config::scalar("z", rotation.z)),
                    Config::map("scale",
                        Config::scalar("x", scale.x),
                        Config::scalar("y", scale.y),
                        Config::scalar("z", scale.z))
                )
    );
    parser(config);

    // Create transformation matrix
    auto two_pi = glm::pi<float>() * 2.0f;
    glm::mat4 transform(1.0);
    transform = glm::scale(transform, scale);
    transform = glm::rotate(transform, rotation.x * two_pi, glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, rotation.y * two_pi, glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, rotation.z * two_pi, glm::vec3(0, 0, 1));
    transform = glm::translate(transform, translation);
    // Assign transformation component with transformation matrix to entity
    auto component = registry.assign<ecs::Transform>(entity, transform);
}

void TransformComponentCtor::construct (entt::DefaultPrototype& prototype, const YAML::Node& config)
{
    glm::vec3 translation;
    glm::vec3 rotation;
    glm::vec3 scale;
    auto parser = Config::make_parser(
                Config::optional(
                    Config::map("position",
                        Config::scalar("x", translation.x),
                        Config::scalar("y", translation.y),
                        Config::scalar("z", translation.z)),
                    Config::map("rotation",
                        Config::scalar("x", rotation.x),
                        Config::scalar("y", rotation.y),
                        Config::scalar("z", rotation.z)),
                    Config::map("scale",
                        Config::scalar("x", scale.x),
                        Config::scalar("y", scale.y),
                        Config::scalar("z", scale.z))
                )
    );
    parser(config);

    // Create transformation matrix
    auto two_pi = glm::pi<float>() * 2.0f;
    glm::mat4 transform(1.0);
    transform = glm::scale(transform, scale);
    transform = glm::rotate(transform, rotation.x * two_pi, glm::vec3(1, 0, 0));
    transform = glm::rotate(transform, rotation.y * two_pi, glm::vec3(0, 1, 0));
    transform = glm::rotate(transform, rotation.z * two_pi, glm::vec3(0, 0, 1));
    transform = glm::translate(transform, translation);
    // Assign transformation component with transformation matrix to entity
    prototype.set<ecs::Transform>(transform);
}

struct ComponentTemplate {
    unsigned size;
    void* buffer;
};
