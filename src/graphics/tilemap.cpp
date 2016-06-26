
#include "graphics/tilemap.h"
#include "util/logging.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

using Shader_t = Shader::Shader;

TileMap::~TileMap () {
    Shader::unload(tileShader);
}

void TileMap::init (const std::vector<std::vector<float>>& map)
{
    tileShader = Shader::load(
        // VERTEX SHADER
        "#version 330 core"
        "   layout(location = 0) in vec2 in_Position;"
        "   layout(location = 1) in vec4 in_Color;"
        "   layout(location = 2) in vec2 in_UV;"
        "   layout(location = 3) in float in_Image;"
        "	uniform mat4 u_projection;"
        "	uniform mat4 u_view;"
        "   out vec2 uv;"
        "   out vec4 color;"
        "   out float image;"
        "   void main() {"
        "     gl_Position =  u_projection * u_view * vec4(in_Position, 0.0, 1.0);"
        "     color = in_Color;"
        "     uv = in_UV;"
        "     image = in_Image;"
        "   }",

        // FRAGMENT SHADER
        "   #version 330 core"
        "   in vec4 color;"
        "   out vec4 fragColor;"
        "   in vec2 uv;"
        "   in float image;"
        "   uniform sampler2DArray u_texture;"
        "   void main(void) {"
        "   fragColor = texture(u_texture, vec3(uv, image)).rgba * color;"
        "   }");

    mesh = Mesh{};
    auto vertices = std::vector<glm::vec2>{};
    auto colours =  std::vector<glm::vec4>{};
    auto texcoords = std::vector<glm::vec2>{};
    auto images = std::vector<float>{};

    // Generate grid
    float x = 0.0f;
    float y = map.size();
    std::size_t w = 0;
    for (auto row : map) {
        for (auto col : row) {
            vertices.push_back(glm::vec2{x,        y       });
            vertices.push_back(glm::vec2{x + 1.0f, y       });
            vertices.push_back(glm::vec2{x + 1.0f, y - 1.0f});
            vertices.push_back(glm::vec2{x,        y - 1.0f});
            colours.push_back(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
            colours.push_back(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
            colours.push_back(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
            colours.push_back(glm::vec4{1.0f, 1.0f, 1.0f, 1.0f});
            texcoords.push_back(glm::vec2{0.0f, 0.0f});
            texcoords.push_back(glm::vec2{1.0f, 0.0f});
            texcoords.push_back(glm::vec2{1.0f, 1.0f});
            texcoords.push_back(glm::vec2{0.0f, 1.0f});
            images.push_back(col);
            images.push_back(col);
            images.push_back(col);
            images.push_back(col);
            x += 1.0f;
        }
        w = std::max(w, row.size());
        x = 0.0f;
        y -= 1.0f;
    }
    // Upload grid data to VBO
    mesh.addBuffer(vertices, true);
    mesh.addBuffer(colours);
    mesh.addBuffer(texcoords);
    imageIdBuffer = mesh.addBuffer(images);
    mesh.addIndexBuffer();

    width = w;
    height = map.size();
    max_index = width * height * 4;
    info("Tilemap loaded: width={} height={}", width, height);

    u_projection = glGetUniformLocation(tileShader.programID, "u_projection");
    u_view = glGetUniformLocation(tileShader.programID, "u_view");
    u_texture = glGetUniformLocation(tileShader.programID, "u_texture");
}

void TileMap::reset (const std::vector<std::vector<float>>& map)
{
    auto images = std::vector<float>{};
    // Generate grid
    for (auto row : map) {
        for (auto col : row) {
            images.push_back(col);
            images.push_back(col);
            images.push_back(col);
            images.push_back(col);
        }
    }
    mesh.setBuffer(imageIdBuffer, images);
}

void TileMap::render (const Rect& bounds)
{
    glUniform1i(u_texture, 0);

    // Generate tilemap indices
    auto indices = std::vector<GLushort>{};
    int base_tile = ((height - int(bounds.top_left.y)) * width) + int(bounds.top_left.x);
    auto base = base_tile * 4;
    auto index = base;

    for (auto y = bounds.top_left.y; y <= bounds.bottom_right.y; ++y) {
        for (auto x = bounds.top_left.x; x < bounds.bottom_right.x; ++x) {
            if (index >= 0.0f && index < max_index) {
                indices.push_back(index);
                indices.push_back(index + 1);
                indices.push_back(index + 2);
                indices.push_back(index + 2);
                indices.push_back(index + 3);
                indices.push_back(index);
            } else {
                // Keep the buffer size the same for orphaning
                indices.push_back(0);
                indices.push_back(0);
                indices.push_back(0);
                indices.push_back(0);
                indices.push_back(0);
                indices.push_back(0);
            }
            index += 4;
        }
        base -= (int)width * 4;
        index = base;
    }

    mesh.drawIndexed(indices);
}
