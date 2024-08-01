#pragma once

#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Image/Cubemap.hpp>

#include <vector>

namespace TabGraph::Renderer {
/// @brief Generates the mips from a_Base filtered using GGX importance sampling
/// @param a_Base the bast cubemap
/// @return the levels generated from a_Base, front being level 1
std::vector<SG::Cubemap> GenerateIBlSpecularMips(
    const SG::Cubemap& a_Base);
}