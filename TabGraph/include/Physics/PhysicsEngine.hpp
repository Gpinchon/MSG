/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 23:37:50
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Physics/Collision.hpp>

#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class RigidBody;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class BroadphaseInterface {
};

class CollisionConfiguration {
};

class CollisionDispatcher {
private:
    CollisionDispatcher() = delete;
    CollisionDispatcher(std::shared_ptr<CollisionDispatcher>);
    std::shared_ptr<CollisionDispatcher> _collisionDispatcher { nullptr };
};

class PhysicsEngine {
public:
    void AddRigidBody(const std::shared_ptr<RigidBody>& rigidBody);
    void Simulate(float step);
    void CheckCollision();

private:
    //TODO Fixe PhysicsEngine
    virtual void _FixedUpdateCPU(float /*delta*/);
    std::vector<Collision::CollideesPair> _collideesPairs;
    std::vector<std::shared_ptr<RigidBody>> _rigidBodies;
};
}