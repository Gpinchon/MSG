#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
struct BoundingSphere {
    glm::vec3 center = { 0.f, 0.f, 0.f };
    float radius     = 0.f;
};

struct BoundingBox {
    glm::vec3 center   = { 0.f, 0.f, 0.f };
    glm::vec3 halfSize = { 0.f, 0.f, 0.f };
};

class BoundingVolume : BoundingBox {
public:
    using BoundingBox::BoundingBox;
    BoundingVolume(const BoundingSphere& a_Sphere)
        : BoundingBox({ .center = a_Sphere.center, .halfSize = glm::vec3(a_Sphere.radius) })
    {
    }
    explicit operator BoundingSphere()
    {
        return { .center = center, .radius = glm::length(halfSize) };
    }
    explicit operator BoundingBox()
    {
        return *this;
    }
};
}
