////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/BoundingVolume.hpp>

#include <glm/mat4x4.hpp>

#include <limits>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
Msg::BoundingVolume::BoundingVolume(const Sphere& a_Sphere)
    : Cube(a_Sphere.center, glm::vec3(a_Sphere.radius))
{
}

float Msg::BoundingVolume::Area() const
{
    return 2.f * glm::length(halfSize);
}

Msg::BoundingVolume::operator Msg::Sphere() const
{
    return { center, glm::length(halfSize) };
}

Msg::BoundingVolume& Msg::BoundingVolume::operator+=(const BoundingVolume& a_Rhs)
{
    if (this != &a_Rhs) {
        SetMinMax(
            glm::min(Min(), a_Rhs.Min()),
            glm::max(Max(), a_Rhs.Max()));
    }
    return *this;
}

Msg::BoundingVolume operator+(const Msg::BoundingVolume& a_Lhs, const Msg::BoundingVolume& a_Rhs)
{
    return Msg::BoundingVolume(a_Lhs) += a_Rhs;
}

Msg::BoundingVolume operator*(const glm::mat4x4& a_Lhs, const Msg::BoundingVolume& a_Rhs)
{
    Msg::BoundingVolume newBV;
    glm::vec3 newMin(std::numeric_limits<float>::max());
    glm::vec3 newMax(std::numeric_limits<float>::lowest());
    for (auto& p : a_Rhs.Points()) {
        glm::vec4 tp = a_Lhs * glm::vec4(p, 1);
        newMin       = glm::min(newMin, glm::vec3(tp) / tp.w);
        newMax       = glm::max(newMax, glm::vec3(tp) / tp.w);
    }
    newBV.SetMinMax(newMin, newMax);
    return newBV;
}