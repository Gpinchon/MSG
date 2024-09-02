#pragma once

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace TabGraph::Tools::BRDFIntegration {
using Color  = glm::vec2;
using Pixels = std::vector<std::vector<Color>>;
enum class Type {
    Standard,
    Sheen
};
/// @brief
/// @param a_Xi random vector (usually Halton23 but could also be Hammersley)
/// @param a_N normal vector
/// @param a_Roughness perceptial roughness
/// @return
glm::vec3 ImportanceSampleGGX(const glm::vec2& a_Xi, const glm::vec3& a_N, const float& a_Roughness);
Pixels Generate(unsigned a_Width, unsigned a_Height, Type a_Type);
}
