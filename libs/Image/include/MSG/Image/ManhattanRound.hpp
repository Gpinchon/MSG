#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cmath>

#include <glm/common.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
inline auto ManhattanDistance(const float& a_X, const float& a_Y)
{
    return std::abs(a_X - a_Y);
}

template <typename T>
inline auto ManhattanDistance(const T& a_X, const T& a_Y)
{
    float dist = 0;
    for (uint32_t i = 0; i < T::length(); i++)
        dist += ManhattanDistance(a_X[i], a_Y[i]);
    return dist;
}

template <typename T>
inline auto ManhattanRound(const T& a_Val)
{
    const auto a      = glm::floor(a_Val);
    const auto b      = glm::ceil(a_Val);
    const auto center = a + 0.5f;
    const auto aDist  = ManhattanDistance(center, a);
    const auto bDist  = ManhattanDistance(center, b);
    return aDist < bDist ? a : b;
}
}