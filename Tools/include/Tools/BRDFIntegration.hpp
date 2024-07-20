#pragma once

#include <vector>

#include <glm/vec2.hpp>

namespace TabGraph::Tools::BRDFIntegration {
using Color  = glm::vec2;
using Pixels = std::vector<std::vector<Color>>;
enum class Type {
    Standard,
    Sheen
};
Pixels Generate(unsigned a_Width, unsigned a_Height, Type a_Type);
}