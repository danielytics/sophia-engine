#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "shader.h"
#include <glm/glm.hpp>

struct Rect {
    glm::vec3 top_left;
    glm::vec3 bottom_right;
};

using Shader_t = Shader::Shader;

class Renderable {
public:
    virtual ~Renderable();
    virtual Shader_t shader ()=0;
    virtual void render (const Rect& bounds)=0;
    virtual Uniform_t projection ()=0;
    virtual Uniform_t view ()=0;
};

#endif // RENDERABLE_H
