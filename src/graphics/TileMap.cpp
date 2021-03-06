
#include "graphics/TileMap.h"
#include "graphics/Debug.h"
#include "util/Logging.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cmath>
#include <algorithm>

using Shader_t = Shader::Shader;

TileMap::~TileMap () {
    tileShader.unload();
}

void TileMap::init (const std::vector<std::vector<float>>& map)
{
    tileShader = Shader::load("data/shaders/tiles.vert", "data/shaders/tiles.frag");
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
    mesh.bind();
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
