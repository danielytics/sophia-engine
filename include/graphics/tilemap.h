#ifndef TILEMAP_H
#define TILEMAP_H

namespace TileMap {

struct Tile {
    glm::vec3 position;
    float image;
    glm::vec4 color;
};
struct Map {
    std::vector<glm::vec3> positions;
    std::vector<float> images;
    std::vector<glm::vec4> colors;
};

VAO&& compileMap(const Map&& map) {
    VAO vao;
    vao.addBuffer(map.colors);
    vao.addBuffer(map.images);
}

}

#endif // TILEMAP_H
