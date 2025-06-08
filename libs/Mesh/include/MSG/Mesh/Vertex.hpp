#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace MSG {
struct Vertex {
    static constexpr auto TexCoordCount = 4;

    glm::vec3 position = { 0, 0, 0 };
    glm::vec3 normal   = { 1, 0, 0 };
    glm::vec4 tangent  = { 0, 1, 0, 0 };
    glm::vec2 texCoord[TexCoordCount];
    glm::vec3 color   = { 1, 1, 1 };
    glm::vec4 joints  = { 0, 0, 0, 0 };
    glm::vec4 weights = { 1, 0, 0, 0 };
};
}