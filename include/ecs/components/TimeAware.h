#ifndef ECS_TIMEAWARE_H
#define ECS_TIMEAWARE_H

namespace ecs {

/**
 * TimeAware component
 * Every entity automatically receives a TimeAware component with timeScale set to 1.0. To use a different value, component can be manually added.
 * absolute and delta allows a system to get timing information for use in triggering events or scaling for framerate independent updates.
 * timeScale is used to scale the timing information in order to allow entities to run in sped up or slowed down time. For example, can be used to
 * implement bullet time for a subset of entities.
 */
struct TimeAware
{
    float timeScale; // floating point value used to scale absolute and delta (eg 0.5 makes the entity seem to run at half speed)
    // static? global? seems this should be shared so it can be set only once at the start of each frame for all entities without iterating through each one
    static float global_time_absolute; // seconds since last reset
    static float global_time_delta; // seconds since last update tick

    float absolute () const {
        return timeScale * global_time_absolute;
    }

    float delta () const {
        return timeScale * global_time_delta;
    }
};

}

#endif // ECS_TIMEAWARE_H
