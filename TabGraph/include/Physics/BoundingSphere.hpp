#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Physics/BoundingElement.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class BoundingSphere : public Core::Inherit<BoundingElement, BoundingSphere> {
    PROPERTY(glm::vec3, Center, 0);
    PROPERTY(float, Radius, 0);
public:
    BoundingSphere(const glm::vec3& center, float radius);
    virtual glm::mat3 LocalInertiaTensor(const float& mass) const override;
    virtual Intersection IntersectRay(const Ray& ray) const override;
    virtual std::set<glm::vec3, compareVec> GetSATAxis(const glm::mat4& transform) const override;
    virtual ProjectionInterval Project(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual std::vector<glm::vec3> Clip(glm::vec3 axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
};
}
