#include "ecs/ctors/Transform.h"
#include "ecs/components/Transform.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util/Helpers.h"
#include "util/Logging.h"

#include <vector>

glm::vec3 toVec (const std::vector<float>& in) {
    return glm::vec3(in[0], in[1], in[2]);
}

void TransformComponentCtor::construct (entt::DefaultPrototype& prototype, const YAML::Node& config)
{
    std::vector<float> translation;
    std::vector<float> rotation;
    std::vector<float> scale;
    auto parser = Config::make_parser(
                Config::optional(Config::sequence("position", translation)),
                Config::optional(Config::sequence("rotation", rotation)),
                Config::optional(Config::sequence("scale", scale))
    );
    parser(config);

    Helpers::pad_with(translation, 3, 0.0f);
    Helpers::pad_with(rotation, 3, 0.0f);
    Helpers::pad_with(scale, 3, 1.0f);

    auto two_pi = glm::pi<float>() * 2.0f;
    lib::transform(rotation.begin(), rotation.end(), rotation.begin(), [two_pi](auto x){return x * two_pi;});
    prototype.set<ecs::Transform>(toVec(translation), toVec(scale), toVec(rotation));
}

