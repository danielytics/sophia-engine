#ifndef RENDERER_H
#define RENDERER_H

#include "lib.h"
#include <glm/glm.hpp>
#include "entt/core/hashed_string.hpp"

namespace graphics {

struct SpriteInstance {
    glm::vec3 scale;
    glm::vec3 rotation;
};

using ShaderMode = entt::HashedString;

namespace shader_modes {

constexpr auto Shadows = "shadows-shader"_hs;
constexpr auto Normal = "normal-shader"_hs;

}

struct RenderMode {
    ShaderMode shaderMode;
};

class Renderer {
public:
    virtual ~Renderer() noexcept = default;

//    virtual void submitMesh (const RenderMode&& renderMode, class MeshRef mesh, class MaterialRef material) = 0;
    virtual void submitSprites (const RenderMode&& renderMode, lib::vector<glm::vec4>&& positions, lib::vector<SpriteInstance>&& instanceData) = 0;

    virtual void commit () = 0;
};

}

#endif // RENDERER_H
