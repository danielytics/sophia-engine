#ifndef ECS_CHARACTERCONTROLLER_H
#define ECS_CHARACTERCONTROLLER_H

#include <entt/core/hashed_string.hpp>
#include <bitset>

namespace ecs {

/**
 * @brief The CharacterController component.
 * This component marks the entity as a controlled character, that is, something
 * which can perform character movements/abilities (walk, jump etc) and is controlled by
 * reading from a sequence of actions. These actions are generated externally from user
 * input or from AI.
 */
struct CharacterController {
    // Unique ID for this controller. Used to route actions.
    entt::HashedString::hash_type uid;

    // Flags controlling abilities of this character.
    // TODO: Should these be made their own components? One component per ability? If an entity has the component, it has the ability?
    // Or perhaps they should be externalised (sub-components?)
    enum {
        CAN_WALK=0,
        CAN_RUN,
        CAN_JUMP,
        CAN_ROLL,
        CAN_CRAWL,
        CAN_DASH,
        CAN_GRAB,
        CAN_WALL_GRAB,
        CAN_WALL_CLIMB,
        CAN_WALL_JUMP,
        NUM_ABILITIES
    };
    std::bitset<NUM_ABILITIES> abilities;

    // Maximum units/second for different abilities
    struct {
        float walk;
        float run;
        float roll;
        float crawl;
        float dash;
        float climb;
    } speeds;

    // Time in seconds from initiation of ability to reach maximum speed
    struct {
        float walk;
        float run;
        float roll;
        float crawl;
        float dash;
        float climb;
    } acceleration_time;

};

}

#endif // ECS_CHARACTERCONTROLLER_H
