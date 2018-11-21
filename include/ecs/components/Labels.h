#ifndef LABELS_H
#define LABELS_H

#include "entt/entt.hpp"

namespace ecs::labels {

// Object casts shadows from shadow casting light sources
using dynamic_shadow = entt::label<"DynamicShadow"_hs>;

// Light source casts shadows from shadow casting objects
using shadow_caster = entt::label<"ShadowCaster"_hs>;

}

#endif // LABELS_H
