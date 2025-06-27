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

/**
 * @brief returns the nearest texel coordinate in accordance to page 256 of OpenGL 4.6 (Core Profile) specs
 * @ref https://registry.khronos.org/OpenGL/specs/gl/glspec46.core.pdf
 */
template <typename T>
inline auto ManhattanRound(const T& a_Val)
{
    return glm::floor(a_Val + 0.5f);
}
}