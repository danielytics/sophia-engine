#ifndef MATERIAL_H
#define MATERIAL_H

#include "graphics/Shader.h"

namespace ecs {

struct Texture {
    unsigned int id;
    Uniform_t location;
};


struct Material
{
    Texture albedo_map;
    Texture normal_map;
    Texture parallax_occlusion_map;
    Texture ambient_occlusion_map;
    Texture roughness_map;
};

}

#endif // MATERIAL_H
