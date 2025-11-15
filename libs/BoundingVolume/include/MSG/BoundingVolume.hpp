#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Cube.hpp>
#include <MSG/Sphere.hpp>

#include <glm/fwd.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
class BoundingVolume : public Cube {
public:
    using Cube::Cube;
    BoundingVolume(const Sphere& a_Sphere);
    BoundingVolume& operator+=(const BoundingVolume& a_Rhs);
    float Area() const;
    operator Sphere() const;
};
}

bool operator==(const Msg::BoundingVolume& a_Lhs, const Msg::BoundingVolume& a_Rhs);
bool operator!=(const Msg::BoundingVolume& a_Lhs, const Msg::BoundingVolume& a_Rhs);
Msg::BoundingVolume operator*(const glm::mat4x4& a_Lhs, const Msg::BoundingVolume& a_Rhs);
Msg::BoundingVolume operator+(const Msg::BoundingVolume& a_Lhs, const Msg::BoundingVolume& a_Rhs);
