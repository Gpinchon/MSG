#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/common.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Cube {
public:
    Cube() = default;
    Cube(const glm::vec3& a_Center, const glm::vec3& a_HalfSize);
    float Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix = { 1.f }) const;
    bool IsInf() const { return glm::any(glm::isinf(halfSize)); }
    glm::vec3 Min() const { return center - halfSize; }
    glm::vec3 Max() const { return center + halfSize; }
    void SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max);
    std::array<glm::vec3, 8> Points() const;
    glm::vec3 center   = { 0.f, 0.f, 0.f };
    glm::vec3 halfSize = { 0.f, 0.f, 0.f };
};
}
