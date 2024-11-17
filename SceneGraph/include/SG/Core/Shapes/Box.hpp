#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/common.hpp>
#include <glm/vec3.hpp>

#include <array>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Box {
public:
    Box() = default;
    Box(const glm::vec3& a_Center, const glm::vec3& a_HalfSize)
        : center(a_Center)
        , halfSize(a_HalfSize)
    {
    }
    bool IsInf() const { return glm::any(glm::isinf(halfSize)); }
    glm::vec3 Min() const { return center - halfSize; }
    glm::vec3 Max() const { return center + halfSize; }
    void SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max)
    {
        halfSize = (a_Max - a_Min) / 2.f;
        for (uint8_t i = 0; i < decltype(halfSize)::length(); i++) {
            if (glm::isinf(halfSize[i]))
                center[i] = 0; // avoid nan
            else
                center[i] = (a_Max[i] + a_Min[i]) / 2.f;
        }
    }
    std::array<glm::vec3, 8> Points() const
    {
        auto minP = Min();
        auto maxP = Max();
        return {
            glm::vec3 { minP.x, minP.y, minP.z },
            glm::vec3 { minP.x, minP.y, maxP.z },
            glm::vec3 { minP.x, maxP.y, maxP.z },
            glm::vec3 { minP.x, maxP.y, minP.z },
            glm::vec3 { maxP.x, maxP.y, maxP.z },
            glm::vec3 { maxP.x, maxP.y, minP.z },
            glm::vec3 { maxP.x, minP.y, minP.z },
            glm::vec3 { maxP.x, minP.y, maxP.z }
        };
    }
    glm::vec3 center   = { 0.f, 0.f, 0.f };
    glm::vec3 halfSize = { 0.f, 0.f, 0.f };
};
}
