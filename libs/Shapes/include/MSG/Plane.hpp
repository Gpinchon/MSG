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
    glm::vec3 GetNormal() const;
    float GetOffset() const;
    glm::vec3 GetPosition() const;
    void SetNormal(const glm::vec3& a_Normal);
    void SetOffset(const float& a_Offset);
    void Normalize();
    float GetDistance(const glm::vec3& a_Point) const;
};
}

inline MSG::Plane::Plane() : glm::vec4(0, 1, 0, 0) {};

inline MSG::Plane::Plane(const glm::vec3& a_Point, const glm::vec3& a_Normal)
    : glm::vec4(glm::normalize(a_Normal), glm::length(a_Point))
{
}

inline glm::vec3 MSG::Plane::GetNormal() const { return { x, y, z }; }

inline float MSG::Plane::GetOffset() const { return w; }

inline glm::vec3 MSG::Plane::GetPosition() const { return GetNormal() + GetOffset(); }

inline void MSG::Plane::SetNormal(const glm::vec3& a_Normal)
{
    x = a_Normal.x;
    y = a_Normal.y;
    z = a_Normal.z;
}
inline void MSG::Plane::SetOffset(const float& a_Offset) { w = a_Offset; }

inline void MSG::Plane::Normalize() { *this /= glm::length(GetNormal()); }

inline float MSG::Plane::GetDistance(const glm::vec3& a_Point) const { return glm::dot(GetNormal(), a_Point) + GetOffset(); }
