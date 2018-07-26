#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include "math/AABB.h"
#include "util/Helpers.h"

class Handle
{
public:
    explicit Handle (const std::size_t idx, class HandledResrouce* const res)
        : index(idx), resources(res) {
    }
    const std::size_t index;
private:
    class HandledResrouce* const resources;

    template <typename T> T& get () const;
    template <typename T> void remove () const;
    template <typename T> friend class HandleRef;
};

template <typename T>
class HandleRef
{
public:
    explicit HandleRef (const Handle& h) : data(h.get<T>()), handle(h) {}
    ~HandleRef () {
        handle.remove<T>();
    }
    T& data;
private:
    const Handle& handle;
};

class BaseManagedResource {
public:
    virtual ~BaseManagedResource();

    inline void remove (const Handle& handle) {
        garbage.push_back(handle.index);
    }

    inline void collectGarbage () {
        std::size_t count = garbage.size();
        for (std::size_t garbage_index=0; garbage_index < count; ++garbage_index) {
            auto g_iter = garbage.begin() + garbage_index;
            if (isUnreferenced(*g_iter)) {
                // Remove from garbage
                auto g_last = garbage.end() - 1;
                if (g_iter != g_last) {
                    *g_iter = std::move(*g_last);
                }
                garbage.pop_back();

                // Remove actual item
                auto idx = handles[*g_iter].index;
                destroy(idx);
                --count;
            } else {
                ++garbage_index;
            }
        }
    }

private:
    struct ResourceHandle {
        // Since we target only 64bit, assume size_t is 64bit
        std::size_t refCount : 16;
        std::size_t index : 48;
    };
    std::vector<ResourceHandle> handles;
    std::vector<std::size_t> garbage;

protected:
    inline std::uint64_t ref (const Handle& h) {
        ResourceHandle& handle = handles[h.index];
        ++handle.refCount;
        return handle.index;
    }
    inline std::size_t insert (std::size_t idx) {
        std::size_t h = handles.size();
        handles.push_back({0, idx});
        return h;
    }
    inline void release (const Handle& h) {
        ResourceHandle& handle = handles[h.index];
        --(handle.refCount);
    }
    inline void update (std::size_t h, std::size_t idx) {
        handles[h].index = idx;
    }
    inline bool isUnreferenced (std::size_t h) {
        return handles[h].refCount;
    }

    virtual void destroy (std::size_t idx) = 0;
};

template <typename T>
class ManagedResource : public BaseManagedResource
{
    struct Item {
        T data;
        std::size_t handleIndex;
    };
public:
    inline Handle add (T&& resource) {
        std::size_t idx = insert(resources.size());
        resources.push_back({std::move(resource), idx});
        return Handle(idx, this);
    }

protected:
    void destroy (std::size_t idx) {
        auto it = resources.begin() + idx;
        auto last = resources.end() - 1;
        if (it != last) {
            // Move the resource
            *it = std::move(*last);
            // Make sure handle points to moved resource
            update(last->handleIndex, idx);
        }
        // Remove garbage item
        resources.pop_back();
    }

private:
    std::vector<Item> resources;


    inline T& get (const Handle& handle) {
        return resources[ref(handle)];
    }
    friend class Handle;
};

template <typename T> T& Handle::get () const
{
    return static_cast<ManagedResource<T>*>(resources)->get(*this);
}

template <typename T> void Handle::remove () const
{
    static_cast<ManagedResource<T>*>(resources)->remove(*this);
}

BaseManagedResource::~BaseManagedResource ()
{

}

struct BoundingCircle {
    glm::vec2 position;
    float radius;
};

struct SceneNodeData {
    AABB aabb;
    std::vector<SceneNodeData>::iterator next;
};

enum ChildDirection {
    NORTH_WEST = 0,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST
};

struct SceneNode {
    AABB aabb;
    Index data;
    Index children[4];
};

class Scene {
public:

    void add (const AABB& aabb)
    {
        glm::vec2 center = aabb.centerPoint();
        allData.push_back({aabb, std::MAX_});
        SceneNodeData* newNode = &allData.back();

    }


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

//        // Insert as child

//    }

private:
    std::vector<SceneNode> allNodes;
    std::vector<SceneNodeData> allData;
};

#endif // SCENE_H
