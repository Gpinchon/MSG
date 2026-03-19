#include <MSG/Cube.hpp>

#include <array>
#include <glm/common.hpp>
#include <glm/vec3.hpp>

Msg::Cube::Cube(const glm::vec3& a_Center, const glm::vec3& a_HalfSize)
    : center(a_Center)
    , halfSize(a_HalfSize)
{
}

bool Msg::Cube::Intersects(const Cube& a_Other)
{
    glm::vec3 aMin = Min();
    glm::vec3 aMax = Max();
    glm::vec3 bMin = a_Other.Min();
    glm::vec3 bMax = a_Other.Max();
    return aMin.x <= bMax.x && aMax.x >= bMin.x
        && aMin.y <= bMax.y && aMax.y >= bMin.y
        && aMin.z <= bMax.z && aMax.z >= bMin.z;
}

bool Msg::Cube::Contains(const Cube& a_Other)
{
    glm::vec3 aMin = Min();
    glm::vec3 aMax = Max();
    glm::vec3 bMin = a_Other.Min();
    glm::vec3 bMax = a_Other.Max();
    return aMin.x <= bMin.x && aMax.x >= bMax.x
        && aMin.y <= bMin.y && aMax.y >= bMax.y
        && aMin.z <= bMin.z && aMax.z >= bMax.z;

    return glm::all(glm::lessThanEqual(glm::abs(center - a_Other.center), (halfSize - a_Other.halfSize)));
}

bool Msg::Cube::Contains(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix) const
{
    return Distance(a_Position, a_TransformMatrix) <= 0.f;
}

float Msg::Cube::Distance(const glm::vec3& a_Position, const glm::mat4x4& a_TransformMatrix) const
{
    glm::vec4 p = a_TransformMatrix * glm::vec4(a_Position - center, 1);
    glm::vec3 d = abs(glm::vec3(p) / p.w) - halfSize;
    return glm::min(glm::max(d.x, glm::max(d.y, d.z)), 0.f) + glm::length(glm::max(d, 0.f));
}

void Msg::Cube::SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max)
{
    halfSize = (a_Max - a_Min) / 2.f;
    for (uint8_t i = 0; i < decltype(halfSize)::length(); i++) {
        if (glm::isinf(halfSize[i]))
            center[i] = 0; // avoid nan
        else
            center[i] = (a_Max[i] + a_Min[i]) / 2.f;
    }
}

std::array<glm::vec3, 8> Msg::Cube::Points() const
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
