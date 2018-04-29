#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "math/AABB.h"
#include "util/Helpers.h"

struct BoundingCircle {
    glm::vec2 position;
    float radius;
};

struct SceneNodeData {

};

enum ChildDirection {
    NORTH_WEST = 0,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST
};

struct SceneNode {
    AABB aabb;
    SceneNodeData* data;
    SceneNode* children[4];
};

class Scene {
public:

//    void add (const BoundingCircle& obj, const SceneNodeData& data) {
//        allData.push_back(data);
//        SceneNode node {
//            {{obj.position.x - obj.radius, obj.position.y + obj.radius},
//             {obj.position.x + obj.radius, obj.position.y - obj.radius}},
//            &allData.back(),
//            {nullptr, nullptr, nullptr, nullptr}
//        };
//        nodes.push_back(std::move(node));
//        SceneNode& newNode = nodes.back();
//        // Find parent node
//        SceneNode& current = nodes.front();
//        if (&current == &newNode) {
//            return;
//        }
//        if (current.data != nullptr) {
//            SceneNode* children[4] = {nullptr, nullptr, nullptr, nullptr};
//            AABB aabb = current.aabb.grow(newNode.aabb);
//            glm::vec2 point = aabb.centerPoint();
//            if (AABB(aabb.rect.top_left, point).contains(newNode.aabb)) {
//                children[NORTH_WEST] = &ne
//            }
//            SceneNode newNode {
//                std::move(aabb),
//                nullptr,
//                {children[NORTH_WEST], children[NORTH_EAST], children[SOUTH_WEST], children[SOUTH_EAST]}
//            };
//            Helpers::move_back_and_replace(nodes, 0, newNode);
//        }

        // Insert as child

    }

private:
    std::vector<SceneNode> nodes;
    std::vector<SceneNodeData> allData;
};

#endif // SCENE_H
