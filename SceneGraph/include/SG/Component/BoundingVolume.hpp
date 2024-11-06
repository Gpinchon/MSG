#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <glm/geometric.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <limits>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
class Plane : public glm::vec4 {
public:
    using glm::vec4::vec4;
    using glm::vec4::operator=;
    Plane()
        : glm::vec4(0, 1, 0, 0) { };
    Plane(const glm::vec3& a_Point, const glm::vec3& a_Normal)
        : glm::vec4(glm::normalize(a_Normal), glm::length(a_Point))
    {
    }
    glm::vec3 GetNormal() const { return { x, y, z }; }
    float GetOffset() const { return w; }
    glm::vec3 GetPosition() const { return GetNormal() + GetOffset(); }
    void SetNormal(const glm::vec3& a_Normal)
    {
        x = a_Normal.x;
        y = a_Normal.y;
        z = a_Normal.z;
    }
    void SetOffset(const float& a_Offset) { w = a_Offset; }
    void Normalize() { *this /= glm::length(GetNormal()); }
    float GetDistance(const glm::vec3& a_Point) const { return glm::dot(GetNormal(), a_Point) + GetOffset(); }
};

class BoundingSphere {
public:
    glm::vec3 center = { 0.f, 0.f, 0.f };
    float radius     = 0.f;
};

class BoundingBox {
public:
    BoundingBox() = default;
    BoundingBox(const glm::vec3& a_Center, const glm::vec3& a_HalfSize)
        : center(a_Center)
        , halfSize(a_HalfSize)
    {
    }
    glm::vec3 Min() const { return center - halfSize; }
    glm::vec3 Max() const { return center + halfSize; }
    void SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max)
    {
        center   = (a_Max + a_Min) / 2.f;
        halfSize = (a_Max - a_Min) / 2.f;
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

class BoundingVolume : public BoundingBox {
public:
    using BoundingBox::BoundingBox;
    BoundingVolume(const BoundingSphere& a_Sphere)
        : BoundingBox(a_Sphere.center, glm::vec3(a_Sphere.radius))
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

    explicit operator BoundingSphere() { return { .center = center, .radius = glm::length(halfSize) }; }
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
