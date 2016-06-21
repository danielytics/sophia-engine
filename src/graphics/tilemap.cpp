
#include "graphics/tilemap.h"
#include "util/logging.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

using Shader_t = Shader::Shader;

TileMap::~TileMap () {
    Shader::unload(shader);
}

void TileMap::init (const std::vector<std::vector<float>>& map)
{
    shader = Shader::load(
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
    float y = 0.0f;
    std::size_t w = 0;
    for (auto row : map) {
        for (auto col : row) {
            vertices.push_back(glm::vec2{x,        y       });
            vertices.push_back(glm::vec2{x + 1.0f, y       });
            vertices.push_back(glm::vec2{x + 1.0f, y - 1.2f});
            vertices.push_back(glm::vec2{x,        y - 1.2f});
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
    mesh.addBuffer(images);
    mesh.addIndexBuffer();

    width = w;
    height = map.size();
    info("Tilemap loaded: width={} height={}", width, height);

    u_projection = glGetUniformLocation(shader.programID, "u_projection");
    u_view = glGetUniformLocation(shader.programID, "u_view");
    u_texture = glGetUniformLocation(shader.programID, "u_texture");
}

void TileMap::render (const glm::vec3& camera, const glm::mat4& projection, const glm::mat4& view)
{
    Shader::use(shader);
    glUniform1i(u_texture, 0);
    glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view));

    // Calculate views bounding rect in world space
    auto ipv = glm::inverse(projection * view);
    auto top_left = ipv * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f);
    auto bottom_right = ipv * glm::vec4(1.0f, -1.0f, 0.0f, 1.0f);

    info("Camera: {}, {}, {}  | bounds: {},{} {},{}", camera.x, camera.y, camera.z, top_left.x, top_left.y, bottom_right.x, bottom_right.y);

    // Generate tilemap indices
    auto indices = std::vector<GLushort>{};
    int xbase = (int)(camera.x - 0.0f);
    int tile_y = (int)(camera.y - 5.0f);
    auto tile_x = xbase;

    auto base_tile = (tile_y * width) + tile_x;
    auto base = base_tile * 4;
    auto index = base;

    for (float index = 0.0f; index < 800.0f; index += 4.0f) {
            indices.push_back(index);
            indices.push_back(index + 1);
            indices.push_back(index + 2);
            indices.push_back(index + 2);
            indices.push_back(index + 3);
            indices.push_back(index);
    }

//    for (auto y = 0.0f; y < 15; ++y) {
//        for (auto x = 0.0f; x < 14; ++x) {
//            if (index >= 0 && index < width * height * 4 && x + camera.x >= 0.0f && x + camera.x < width) {
//                indices.push_back(index);
//                indices.push_back(index + 1);
//                indices.push_back(index + 2);
//                indices.push_back(index + 2);
//                indices.push_back(index + 3);
//                indices.push_back(index);
//            } else {
//                indices.push_back(0);
//                indices.push_back(0);
//                indices.push_back(0);
//                indices.push_back(0);
//                indices.push_back(0);
//                indices.push_back(0);
//            }
//            index += 4;
//        }
//        base += (int)width * 4;
//        index = base;
//    }

    mesh.drawIndexed(indices);
}
