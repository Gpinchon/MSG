#pragma once

#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Image/Cubemap.hpp>

#include <vector>

namespace TabGraph::Renderer {
/// @brief Generates the mips from a_Base filtered using GGX importance sampling
/// @param a_Base the bast cubemap
/// @param a_Width, a_Height : the size of the resulting cubemap
/// @return the levels generated from a_Base, front being the resized a_Base
std::vector<SG::Cubemap> GenerateIBlSpecularMips(
    const unsigned& a_Width,
    const unsigned& a_Height,
    const SG::Cubemap& a_Base);
}