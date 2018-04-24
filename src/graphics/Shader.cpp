
#include "graphics/Shader.h"
#include "util/Logging.h"

#include <fstream>

GLuint compileAndAttach (GLuint shaderProgram, GLenum programType, const std::string& shaderSource)
{
    GLuint program = glCreateShader(programType);

    // Compile the shader
    char* source = const_cast<char*>(shaderSource.c_str());
    int32_t size = int32_t(shaderSource.length());
    glShaderSource(program, 1, &source, &size);
    glCompileShader(program);

    // Check for compile errors
    int wasCompiled = 0;
    glGetShaderiv(program, GL_COMPILE_STATUS, &wasCompiled);
    if (wasCompiled == 0)
    {
        // Find length of shader info log
        int maxLength;
        glGetShaderiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // Get shader info log
        char* shaderInfoLog = new char[maxLength];
        glGetShaderInfoLog(program, maxLength, &maxLength, shaderInfoLog );

        warn("Failed to compile shader: {}", shaderInfoLog);

        delete [] shaderInfoLog;

        // Signal error
        return GLuint(-1);
    }

    // Attach the compiled program
    glAttachShader(shaderProgram, program);
    return program;
}

Shader::Shader createShader (const std::string& vertexShader, const std::string& fragmentShader)
{
    GLuint shaderProgram = glCreateProgram();
    glBindAttribLocation(shaderProgram, 0, "in_Position");
    glBindAttribLocation(shaderProgram, 1, "in_Color");

    // Compile shader programs
    GLuint vertexProgram = compileAndAttach(shaderProgram, GL_VERTEX_SHADER, vertexShader);
    GLuint fragmentProgram = compileAndAttach(shaderProgram, GL_FRAGMENT_SHADER, fragmentShader);

    // Link the shader programs into one
    glLinkProgram(shaderProgram);
    int isLinked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, reinterpret_cast<int*>(&isLinked));
    if (!isLinked) {
        // Find length of shader info log
        int maxLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // Get shader info log
        char* shaderProgramInfoLog = new char[maxLength];
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);

        warn("Shader linking failed: {}", shaderProgramInfoLog);

        delete [] shaderProgramInfoLog;

        return {};
    }

    return {shaderProgram, vertexProgram, fragmentProgram};
}

Shader::Shader Shader::load (const std::string& vertexShaderFilename, const std::string& fragmentShaderFilename) {
    std::ifstream vertexShaderFile { vertexShaderFilename };
    std::string vertexShaderSource { std::istreambuf_iterator<char>(vertexShaderFile), std::istreambuf_iterator<char>() };
    std::ifstream fragmentShaderFile { fragmentShaderFilename };
    std::string fragmentShaderSource { std::istreambuf_iterator<char>(fragmentShaderFile), std::istreambuf_iterator<char>() };
    return createShader(vertexShaderSource, fragmentShaderSource);
}


void Shader::Shader::unload () const
{
    glUseProgram(0);
    glDetachShader(programID, vertexProgram);
    glDetachShader(programID, fragmentProgram);
    glDeleteProgram(programID);
    glDeleteShader(vertexProgram);
    glDeleteShader(fragmentProgram);
}

void Shader::Shader::bindUnfiromBlock(const std::string& blockName, unsigned int bindingPoint) const
{
    GLuint location = glGetUniformBlockIndex(programID, blockName.c_str());
    glUniformBlockBinding(programID, location, bindingPoint);
}

Uniform_t Shader::Shader::uniform(const std::string& name) const
{
    return glGetUniformLocation(programID, name.c_str());
}
