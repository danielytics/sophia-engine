#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "Shader.h"
#include "math/Types.h"

using Shader_t = Shader::Shader;

class Renderable {
public:
    virtual ~Renderable();
    virtual void render (const Rect& bounds)=0;
};

#endif // RENDERABLE_H
