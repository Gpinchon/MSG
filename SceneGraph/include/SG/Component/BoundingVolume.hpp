#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Shapes/Box.hpp>
#include <SG/Core/Shapes/Sphere.hpp>

#include <glm/mat4x4.hpp>

#include <limits>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
class BoundingVolume : public Box {
public:
    using Box::Box;
    BoundingVolume(const Sphere& a_Sphere)
        : Box(a_Sphere.center, glm::vec3(a_Sphere.radius))
    {
    }

    BoundingVolume& operator+=(const BoundingVolume& a_Rhs)
    {
        if (this != &a_Rhs) {
            SetMinMax(
                glm::min(Min(), a_Rhs.Min()),
                glm::max(Max(), a_Rhs.Max()));
        }
        return *this;
    }
    BoundingVolume operator+(const BoundingVolume& a_Rhs)
    {
        BoundingVolume newBV = *this;
        return newBV += a_Rhs;
    }

    operator Sphere() const { return { .center = center, .radius = glm::length(halfSize) }; }
};

inline BoundingVolume operator*(const glm::mat4x4& a_Rhs, const BoundingVolume& a_Lhs)
{
    BoundingVolume newBV;
    glm::vec3 newMin(std::numeric_limits<float>::max());
    glm::vec3 newMax(std::numeric_limits<float>::lowest());
    for (auto& p : a_Lhs.Points()) {
        glm::vec4 tp = a_Rhs * glm::vec4(p, 1);
        newMin       = glm::min(newMin, glm::vec3(tp) / tp.w);
        newMax       = glm::max(newMax, glm::vec3(tp) / tp.w);
    }
    newBV.SetMinMax(newMin, newMax);
    return newBV;
}
}
