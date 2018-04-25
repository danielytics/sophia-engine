#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Shader.h"
#include <glm/glm.hpp>

struct Rect {
    glm::vec3 top_left;
    glm::vec3 bottom_right;
};

using Shader_t = Shader::Shader;

class Renderable {
public:
    virtual ~Renderable();
    virtual void render (const Rect& bounds)=0;
};

#endif // RENDERABLE_H
