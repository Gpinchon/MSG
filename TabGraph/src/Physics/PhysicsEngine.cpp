/*
* @Author: gpinchon
* @Date:   2020-08-07 18:36:53
* @Last Modified by:   gpinchon
* @Last Modified time: 2020-08-18 23:28:41
*/
#include <Physics/PhysicsEngine.hpp>
#include <Nodes/Node.hpp>
#include <Physics/IntersectFunctions.hpp>
#include <Physics/RigidBody.hpp>
#include <Physics/CollisionAlgorithmGJKRaycast.hpp>
#include <Physics/CollisionAlgorithmSAT.hpp>
#include <Physics/Collision.hpp>

#include <algorithm>
#include <glm/gtx/norm.hpp>
#include <iostream>


namespace TabGraph::Physics {
void PhysicsEngine::_FixedUpdateCPU(float step)
{
    Simulate(step);
    CheckCollision();
}

void PhysicsEngine::AddRigidBody(const std::shared_ptr<RigidBody>& rigidBody)
{
    _rigidBodies.push_back(rigidBody);
}

float lastStep = 0.f;

void SimulateBody(std::shared_ptr<RigidBody> rigidBody, float step)
{
    rigidBody->IntegrateVelocities(step);
    auto node(rigidBody->GetNode());
    rigidBody->SetLastPosition(rigidBody->GetLocalPosition());
    rigidBody->SetLastRotation(rigidBody->GetLocalRotation());
    if (node != nullptr) {
        rigidBody->SetLocalPosition(node->GetWorldPosition());
        rigidBody->SetLocalRotation(node->GetWorldRotation());
    }

    rigidBody->SetLocalPosition(rigidBody->GetLocalPosition() + rigidBody->GetLinearVelocity() * step);
    rigidBody->SetLocalRotation(normalize(rigidBody->GetLocalRotation() + step * 0.5f * rigidBody->AngularSpin() * rigidBody->GetLocalRotation()));
}

void PhysicsEngine::Simulate(float step)
{
    for (auto& rigidBody : _rigidBodies) {
        SimulateBody(rigidBody, step);
    }
    lastStep = step;
}

static std::vector<Collision> collisionList;
static std::vector<Collision> cachedCollisionList;

void SignalCollision(const Collision& collision)
{
    if (std::find(collisionList.begin(), collisionList.end(), collision) == collisionList.end()) {
        collisionList.push_back(collision);
    }
}

auto PointVelocity(std::shared_ptr<RigidBody> a, const glm::vec3& p)
{
    auto r = p - a->GetLocalPosition();
    return a->GetLinearVelocity() + glm::cross(a->GetAngularVelocity(), r);
}

enum CollisionType {
    MovingAway,
    Resting,
    Colliding
};

auto colliding(const Collision& c)
{
    auto pair = c.GetCollideesPair();
    auto a = pair.GetFirst();
    auto b = pair.GetSecond();
    auto vp1 = PointVelocity(a, c.GetPosition()), vp2 = PointVelocity(b, c.GetPosition());

    auto vrel = glm::dot(c.GetNormal(), (vp1 - vp2));

    //std::cout << "vrel : " << vrel << '\n';

    if (vrel > 0.0001) /* moving away */ {
        //std::cout << " moving away\n";
        return MovingAway;
    }
    if (vrel > -0.0001) /* resting contact */ {
        //std::cout << " resting contact\n";
        return Resting;
    } else /* vrel < -THRESHOLD */ {
        //std::cout << " colliding\n";
        return Colliding;
    }
}

auto computeImpulseDenominator(std::shared_ptr<RigidBody> rigidBody, const glm::vec3& pos, const glm::vec3& normal)
{
    auto r0 = pos - rigidBody->GetLocalPosition();
    auto c0 = glm::cross(r0, normal);
    auto vec = glm::cross(c0 * rigidBody->InvInertiaTensor(), r0);

    return rigidBody->GetInvMass() + glm::dot(normal, vec);
}

std::map<std::shared_ptr<RigidBody>, std::pair<glm::vec3, glm::vec3>> totalImpulses;

void HandleCollision(Collision collision)
{
    auto pair = collision.GetCollideesPair();
    auto a = pair.GetFirst();
    auto b = pair.GetSecond();

    auto n = collision.GetNormal();
    auto e = std::min(a->GetRestitution(), b->GetRestitution());

    auto I1 = a->InvInertiaTensor();
    auto I2 = b->InvInertiaTensor();
    auto w1 = a->GetAngularVelocity();
    auto w2 = b->GetAngularVelocity();
    auto v1 = a->GetLinearVelocity();
    auto v2 = b->GetLinearVelocity();
    auto r1 = collision.GetPosition() - a->GetLocalPosition();
    auto r2 = collision.GetPosition() - b->GetLocalPosition();
    auto vp1 = v1 + cross(w1, r1);
    auto vp2 = v2 + cross(w2, r2);
    auto vr = vp2 - vp1;
    auto ri1 = cross(I1 * cross(r1, n), r1);
    auto ri2 = cross(I2 * cross(r2, n), r2);
    auto im1 = a->GetInvMass();
    auto im2 = b->GetInvMass();
    auto ja = -(1.f + e) * dot(vr, n);
    auto jb = im1 + im2 + dot(ri1 + ri2, n);
    auto j = ja / jb;


    if (!a->GetStatic()) {
        auto v = v1 - j * im1 * n;
        auto w = w1 - j * I1 * cross(r1, n);
        a->SetLinearVelocity(v);
        a->SetAngularVelocity(w);
    }

    if (!b->GetStatic()) {
        auto v = v2 + j * im2 * n;
        auto w = w2 + j * I2 * cross(r2, n);
        b->SetLinearVelocity(v);
        b->SetAngularVelocity(w);
    }
}

void HandleCollisions()
{
    bool collisionFound;
    do {
        collisionFound = false;
        for (const auto& collision : collisionList) {
            auto collisionType = colliding(collision);
            if (collisionType == Colliding) {
                collisionFound = true;
                HandleCollision(collision);
            } else if (collisionType == Resting) {
                HandleCollision(collision);
            }
        }
        for (auto impulse : totalImpulses) {
            auto a = impulse.first;
            a->SetLinearVelocity(a->GetLinearVelocity() + impulse.second.first);
            a->SetAngularVelocity(a->GetAngularFactor() + impulse.second.second);
        }
        totalImpulses.clear();
    } while (collisionFound);
    totalImpulses.clear();
    collisionList.clear();
}

void PhysicsEngine::CheckCollision()
{
    CollisionAlgorithmGJKRaycast collisionAlgorithm;
    for (auto& a : _rigidBodies) {
        for (auto& b : _rigidBodies) {
            if (a == b)
                continue;
            Collision::CollideesPair pair(a, b);
            if (std::find(_collideesPairs.begin(), _collideesPairs.end(), pair) != _collideesPairs.end())
                continue;
            Collision out;
            while (collisionAlgorithm.Collides(a, b, out) && colliding(out) != Resting) {
                _collideesPairs.push_back(pair);
                auto finalFraction = collisionAlgorithm.GetFraction() - 0.1f;
                if (!a->GetStatic()) {
                    a->SetLocalPosition(mix(a->GetLastPosition(), a->GetLocalPosition(), finalFraction));
                    a->SetLocalRotation(mix(a->GetLastRotation(), a->GetLocalRotation(), finalFraction));
                }
                if (!b->GetStatic()) {
                    b->SetLocalPosition(mix(b->GetLastPosition(), b->GetLocalPosition(), finalFraction));
                    b->SetLocalRotation(mix(b->GetLastRotation(), b->GetLocalRotation(), finalFraction));
                }
                HandleCollision(out);
                CheckCollision();
                return;
            }
        }
    }
    for (auto& rigidBody : _rigidBodies) {
        rigidBody->GetNode()->SetLocalPosition(rigidBody->GetLocalPosition());
        rigidBody->GetNode()->SetLocalRotation(normalize(rigidBody->GetLocalRotation()));
    }
    _collideesPairs.clear();
}
}