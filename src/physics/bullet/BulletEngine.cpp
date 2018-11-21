#include "physics/Engine.h"
#include "util/Logging.h"

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btBox2dShape.h>


struct physics::PhysicsData
{
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btBroadphaseInterface* broadphase;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* dynamicsWorld;

    btAlignedObjectArray<btCollisionShape*> collisionShapes;
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


void physics::Engine::addBody (const glm::vec2& position, const glm::vec2& halfExtents)
{
    btCollisionShape* groundShape = new btBox2dShape(btVector3(btScalar(halfExtents.x), btScalar(halfExtents.y), btScalar(0)));

    data->collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(position.x, position.y, 0));

    btScalar mass(0.);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        groundShape->calculateLocalInertia(mass, localInertia);

    //using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    //add the body to the dynamics world
    data->dynamicsWorld->addRigidBody(body);
}
