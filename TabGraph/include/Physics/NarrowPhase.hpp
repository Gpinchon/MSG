#pragma once

#include "Physics/Collision.hpp"
#include "Physics/CollisionAlgorithm.hpp"
#include <memory>
#include <unordered_map>
#include <unordered_set>

//class CollisionAlgorithm;
//class Collision;
class RigidBody;

class NarrowPhase {
public:
    NarrowPhase(CollisionAlgorithm& collisionAlgorithm);
    void SignalCollision(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b)
    {
        _collisionsToCheck[a].insert(b);
    }
    void CheckCollisions()
    {
        for (const auto& c : _collisionsToCheck) {
            const auto& a = c.first;
            for (const auto& b : c.second) {
                Collision out;
                if (_collisionAlgorithm.Collides(a, b, out)) {
                    _collisions.insert(out);
                }
            }
        }
    };

private:
    CollisionAlgorithm& _collisionAlgorithm;
    std::unordered_set<Collision> _collisions;
    std::unordered_map<std::shared_ptr<RigidBody>, std::unordered_set<std::shared_ptr<RigidBody>>> _collisionsToCheck;
};