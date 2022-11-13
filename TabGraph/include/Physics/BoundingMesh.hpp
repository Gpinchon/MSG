#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Physics/BoundingElement.hpp>
#include <Core/Inherit.hpp>
#include <Core/Property.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Shapes {
class Mesh;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Physics {
class BoundingMesh : public Core::Inherit<BoundingElement, BoundingMesh> {
    PROPERTY(std::shared_ptr<Shapes::Mesh>, Mesh, nullptr);
public:
    BoundingMesh(const std::shared_ptr<Shapes::Mesh>& mesh);
    virtual glm::mat3 LocalInertiaTensor(const float& mass) const override;
    virtual std::set<glm::vec3, compareVec> GetSATAxis(const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual ProjectionInterval Project(const glm::vec3& axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
    virtual std::vector<glm::vec3> Clip(glm::vec3 axis, const glm::mat4& transform = glm::mat4(1.f)) const override;
};
}