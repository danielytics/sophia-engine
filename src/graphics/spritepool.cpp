
#include "graphics/spritepool.h"
#include "util/logging.h"

using Shader_t = Shader::Shader;

SpritePool::~SpritePool () {
    Shader::unload(spriteShader);
}

void SpritePool::init ()
{
    spriteShader = Shader::load(
        // VERTEX SHADER
        "#version 330 core"
        "   layout(location = 0) in vec2 in_Position;"
        "   layout(location = 1) in vec2 in_UV;"
        "   uniform samplerBuffer u_tbo_tex;"
        "	uniform mat4 u_projection;"
        "	uniform mat4 u_view;"
        "   out vec2 uv;"
        "   out float image;"
        "   out vec4 color;"
        "   void main() {"
        "     int offset = gl_InstanceID * 3;"
        "     float x  = texelFetch(u_tbo_tex, offset + 0).r;"
        "     float y  = texelFetch(u_tbo_tex, offset + 1).r;"
        "     image = texelFetch(u_tbo_tex, offset + 2).r;"
        "     gl_Position = u_projection * u_view * vec4(in_Position.x + x, in_Position.y + y, 0.0, 1.0);"
        "     uv = in_UV;"
        "   }",

        // FRAGMENT SHADER
        "   #version 330 core"
        "   in vec2 uv;"
        "   in float image;"
        "   out vec4 fragColor;"
        "   uniform sampler2DArray u_texture;"
        "   void main(void) {"
        "     fragColor = texture(u_texture, vec3(uv, image)).rgba;"
        "   }");

    mesh = Mesh{};
    mesh.addBuffer(std::vector<glm::vec3>{
            {-0.5f, 0.5f, 0.5f},
            { 0.5f, 0.5f, 0.5f},
            { 0.5f,-0.5f, 0.5f},
            { 0.5f,-0.5f, 0.5f},
            {-0.5f,-0.5f, 0.5f},
            {-0.5f, 0.5f, 0.5f},
        }, true);
    mesh.addBuffer(std::vector<glm::vec2>{
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {1.0f, 1.0f},
            {1.0f, 1.0f},
            {0.0f, 1.0f},
            {0.0f, 0.0f},
        });

    glGenBuffers(1, &tbo);
    glBindBuffer(GL_TEXTURE_BUFFER, tbo);
    glGenTextures(1, &tbo_tex);

    u_tbo_tex = glGetUniformLocation(spriteShader.programID, "u_tbo_tex");
    u_projection = glGetUniformLocation(spriteShader.programID, "u_projection");
    u_view = glGetUniformLocation(spriteShader.programID, "u_view");
    u_texture = glGetUniformLocation(spriteShader.programID, "u_texture");

    spriteCount = 0;
}

void SpritePool::update (const std::vector<Sprite>& sprites)
{
    spriteCount = sprites.size();
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_BUFFER, tbo_tex);
    glBufferData(GL_TEXTURE_BUFFER, sizeof(glm::vec3) * spriteCount, reinterpret_cast<const float*>(sprites.data()), GL_STREAM_DRAW);
    glTexBuffer(GL_TEXTURE_BUFFER, GL_R32F, tbo);
}

void SpritePool::render (const Rect& bounds)
{
    info("Rendering {} sprites", spriteCount);
    glUniform1i(u_texture, 0);
    glUniform1i(u_tbo_tex, 1);
    mesh.draw(spriteCount);
}
