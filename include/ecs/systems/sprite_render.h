#ifndef SPRITE_RENDER_H
#define SPRITE_RENDER_H

#include "ecs/systems/System.h"

#include "lib.h"
#include <glm/glm.hpp>

#include "graphics/Renderer.h"

#include "ecs/components/Transform.h"
#include "ecs/components/Sprite.h"

namespace systems {

template <typename... Components>
class sprite_render_system : public ecs::system<sprite_render_system<Components...>, ecs::Transform, ecs::Sprite, Components...> {
public:
    sprite_render_system (graphics::Renderer& renderer)
        : renderer(renderer)
    {

    }

    ~sprite_render_system() noexcept = default;

    void update (ecs::entity, const ecs::Transform& xform, const ecs::Sprite& sprite) {
        // gather commands for renderer
        spheres.push_back(glm::vec4(xform.position, 1.0f));
        instances.push_back({xform.scale, xform.rotation});
    }

    void post () {
        std::size_t num_objects = spheres.size();
        // submit commands to renderer
        renderer.submitSprites({graphics::shader_modes::Normal}, std::move(spheres), std::move(instances));
        // reset for next frame
        spheres = {};
        instances = {};
        // next frame will likely have the same number of sprites to render
        spheres.reserve(num_objects);
        instances.reserve(num_objects);
    }

private:
    graphics::Renderer& renderer;
    lib::vector<glm::vec4> spheres; // x, y, z, radius
    lib::vector<graphics::SpriteInstance> instances;
};

}

#endif // SPRITE_RENDER_H
