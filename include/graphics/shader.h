#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>

namespace Shader {
    struct Shader {
        GLuint programID;
        GLuint vertexProgram;
        GLuint fragmentProgram;
    };

    Shader load (const std::string& vertexShader, const std::string& fragmentShader);
    void unload(const Shader&);

    inline void use (const Shader& shader) {
        glUseProgram(shader.programID);
    }
}

#endif // SHADER_H
