#ifndef SYSTEM_H
#define SYSTEM_H

#include "lib.h"
#include "entt/entity/registry.hpp"
#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include "tbb/concurrent_unordered_set.h"
#include "tbb/concurrent_vector.h"

namespace ecs {

typedef entt::DefaultRegistry::entity_type entity;

class System {
public:
    virtual ~System () noexcept = default;

//    virtual void start (entt::DefaultRegistry& registry) = 0;
    virtual void run (entt::DefaultRegistry& registry) = 0;
//    virtual void pause () = 0;
//    virtual void resume () = 0;
//    virtual void stop () = 0;
};

enum class EntityNotification {
    ADDED,
    REMOVED
};

namespace detail {
    template<typename T> struct has_method__pre {
    private:
        typedef std::true_type yes;
        typedef std::false_type no;
        template<typename U> static auto test(int) -> decltype(std::declval<U>().pre() == 1, yes());
        template<typename> static no test(...);
    public:
        static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
    };
    template<typename T> typename std::enable_if<has_method__pre<T>::value, void>::type call_if_declared__pre(T* self) {self->pre();}
    void call_if_declared__pre(...) {}

    template<typename T> struct has_method__post {
    private:
        typedef std::true_type yes;
        typedef std::false_type no;
        template<typename U> static auto test(int) -> decltype(std::declval<U>().post() == 1, yes());
        template<typename> static no test(...);
    public:
        static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
    };
    template<typename T> typename std::enable_if<has_method__post<T>::value, void>::type call_if_declared__post(T* self) {self->post();}
    void call_if_declared__post(...) {}

    template<typename T> struct has_method__notify {
    private:
        typedef std::true_type yes;
        typedef std::false_type no;
        template<typename U> static auto test(int) -> decltype(std::declval<U>().notify(EntityNotification::ADDED, lib::vector<entity>{}) == 1, yes());
        template<typename> static no test(...);
    public:
        static constexpr bool value = std::is_same<decltype(test<T>(0)),yes>::value;
    };
    template<typename T> typename std::enable_if<has_method__post<T>::value, void>::type call_if_declared__notify(T* self, EntityNotification n, lib::vector<entity> e) {self->notify(n, e);}
    void call_if_declared__notify(...) {}
}

template <class This, typename... Components>
class system : public System {
public:
    system()
        : parallel(false)
        , notificationsEnabled(false) {

    }
    virtual ~system() noexcept = default;

    void run (entt::DefaultRegistry& registry) {
        lib::vector<entity> added;
        lib::vector<entity> removed;
        detail::call_if_declared__pre(static_cast<This*>(this));
        if (parallel) {
            auto view = registry.view<Components...>(entt::persistent_t{});
            tbb::concurrent_vector<entity> updatedEntities;
            tbb::parallel_for(tbb::blocked_range<std::size_t>(0, view.size()), [this,view,&updatedEntities](const tbb::blocked_range<size_t>& range){
                auto iter = view.begin() + range.begin();
                for (auto i = range.begin(); i != range.end(); ++i) {
                    auto entity = *iter++;
                    addLiveEntity(updatedEntities, entity);
                    static_cast<This*>(this)->update(entity, (view.template get<Components>(entity))...);
                }
            });
            findAddedAndRemovedEntities(updatedEntities, added, removed);
        } else {
            lib::vector<entity> updatedEntities;
            registry.view<Components...>().each([this,&updatedEntities](auto entity, const Components... args){
                addLiveEntity(updatedEntities, entity);
                static_cast<This*>(this)->update(entity, args...);
            });
            findAddedAndRemovedEntities(updatedEntities, added, removed);
        }
        // Compiled away if This::notify(n,e) is not defined
        if constexpr (detail::has_method__notify<This>::value) {
            if (notificationsEnabled) {
                detail::call_if_declared__notify(static_cast<This*>(this), EntityNotification::REMOVED, removed);
                detail::call_if_declared__notify(static_cast<This*>(this), EntityNotification::ADDED, added);
            }
        }
        detail::call_if_declared__post(static_cast<This*>(this));
    }

protected:
    bool notificationsEnabled;

private:
    bool parallel;
    lib::vector<entity> liveEntities;

    template <typename T>
    inline void addLiveEntity (T& current, entity e) {
        // Compiled away if This::notify(n,e) is not defined
        if constexpr (detail::has_method__notify<This>::value) {
            current.push_back(e);
        }
    }

    template <typename T>
    inline void findAddedAndRemovedEntities (T& current, lib::vector<entity>& added, lib::vector<entity>& removed) {
        // Compiled away if This::notify(n,e) is not defined
        if constexpr (detail::has_method__notify<This>::value) {
            if (notificationsEnabled) {
                lib::set_difference(current.begin(), current.end(), liveEntities.begin(), liveEntities.end(), lib::back_inserter(added));
                lib::set_difference(liveEntities.begin(), liveEntities.end(), current.begin(), current.end(), lib::back_inserter(removed));
                lib::copy(current.begin(), current.end(), lib::back_inserter(liveEntities));
            }
        }
    }
};

}

#endif // SYSTEM_H
