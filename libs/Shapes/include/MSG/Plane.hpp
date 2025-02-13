#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Plane : public glm::vec4 {
public:
    using glm::vec4::vec4;
    using glm::vec4::operator=;
    Plane();
    Plane(const glm::vec3& a_Point, const glm::vec3& a_Normal);
    glm::vec3 GetNormal() const { return { x, y, z }; }
    float GetOffset() const { return w; }
    glm::vec3 GetPosition() const { return GetNormal() + GetOffset(); }
    void SetNormal(const glm::vec3& a_Normal);
    void SetOffset(const float& a_Offset) { w = a_Offset; }
    void Normalize() { *this /= glm::length(GetNormal()); }
    float GetDistance(const glm::vec3& a_Point) const { return glm::dot(GetNormal(), a_Point) + GetOffset(); }
};
}
