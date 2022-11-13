#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Physics/CollisionAlgorithm.hpp>
#include <Core/Property.hpp>

#include <glm/glm.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class CollisionAlgorithmGJKRaycast : public CollisionAlgorithm {
    PROPERTY(float, AllowedPenetration, 0);
public:
    class IntersectionResult {
        PROPERTY(float, Fraction, 1);
        PROPERTY(float, AllowedPenetration, 0);
        PROPERTY(float, PenetrationDepth, 0);
        PROPERTY(glm::vec3, Normal, 0);
        PROPERTY(glm::vec3, HitPointA, std::numeric_limits<float>::infinity());
        PROPERTY(glm::vec3, HitPointB, std::numeric_limits<float>::infinity());
    public:
        auto HitPoint() const { return (GetHitPointA() + GetHitPointB()) / 2.f; }
    };
    virtual bool Collides(const std::shared_ptr<RigidBody>& a, const std::shared_ptr<RigidBody>& b, Collision& out) override;
    float GetFraction() const;

private:
    IntersectionResult _intersectionResult;
};
}