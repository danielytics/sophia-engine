#include "physics/Engine.h"
#include "util/Logging.h"

#include <btBulletDynamicsCommon.h>

struct physics::PhysicsData
{
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;
};

physics::Engine::Engine ()
    : data(nullptr)
{

}

void physics::Engine::init (const YAML::Node& config_node)
{
    info("Starting Bullet physics engine");
    data = new physics::PhysicsData;
    data->collisionConfiguration = new btDefaultCollisionConfiguration();
    data->dispatcher = new btCollisionDispatcher(data->collisionConfiguration);
    data->broadphase = new btDbvtBroadphase();
    data->solver = new btSequentialImpulseConstraintSolver();
    data->dynamicsWorld = new btDiscreteDynamicsWorld(
                data->dispatcher,
                data->broadphase,
                data->solver,
                data->collisionConfiguration);
    data->dynamicsWorld->setGravity(btVector3(0 , -10 , 0));
}

physics::Engine::~Engine ()
{
    info("Stopping Bullet physics engine");

    if (data) {
        delete data->dynamicsWorld;
        delete data->solver;
        delete data->broadphase;
        delete data->dispatcher;
        delete data->collisionConfiguration;

        delete data;
    }
}

void physics::Engine::step (float dt)
{

}
