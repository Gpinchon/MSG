#pragma once

#include <array>
#include <vector>

namespace TabGraph::Tools::BRDFIntegration {
using Color  = std::array<std::byte, 3>;
using Pixels = std::vector<std::vector<Color>>;
enum class Type {
    Standard,
    Sheen
};
Pixels Generate(unsigned a_Width, unsigned a_Height, Type a_Type);
}