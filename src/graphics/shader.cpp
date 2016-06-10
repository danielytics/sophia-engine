
#include "graphics/shader.h"
#include "util/logging.h"

GLuint compileAndAttach (GLuint shaderProgram, GLenum programType, const std::string& shaderSource)
{
    GLuint program = glCreateShader(programType);

    // Compile the shader
    char* source = const_cast<char*>(shaderSource.c_str());
    int32_t size = shaderSource.length();
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
        return -1;
    }

    // Attach the compiled program
    glAttachShader(shaderProgram, program);
    return program;
}

Shader::Shader Shader::load (const std::string& vertexShader, const std::string& fragmentShader)
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
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int*)&isLinked);
    if (!isLinked) {
        // Find length of shader info log
        int maxLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

        // Get shader info log
        char* shaderProgramInfoLog = new char[maxLength];
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);

        warn("Shader linking failed: {}", shaderProgramInfoLog);

        delete [] shaderProgramInfoLog;

        return Shader::Shader{};
    }

    return Shader::Shader{shaderProgram, vertexProgram, fragmentProgram};
}

void Shader::unload (const Shader::Shader& shader)
{
    glUseProgram(0);
    glDetachShader(shader.programID, shader.vertexProgram);
    glDetachShader(shader.programID, shader.fragmentProgram);
    glDeleteProgram(shader.programID);
    glDeleteShader(shader.vertexProgram);
    glDeleteShader(shader.fragmentProgram);
}
