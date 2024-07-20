#pragma once

#include <glm/glm.hpp>
#include <vector>

/**
 * @ref http://filmicworlds.com/blog/optimizing-ggx-shaders-with-dotlh/
 */
namespace TabGraph::Tools::GGXLUT {
using Color  = glm::vec3;
using Pixels = std::vector<std::vector<Color>>;
// Generates the GGX FV LUT, x = LdotH, y = BRDF alpha
Pixels GenerateFV(unsigned a_Width, unsigned a_Height);
// Generates the GGX D LUT, x = NdotH, y = BRDF alpha
Pixels GenerateD(unsigned a_Width, unsigned a_Height);
/**
 * @brief Generates the GGX FV & D LUT
 * The GGX FV LUT is stored in the RG channels
 * The GGX D LUT is stored in the B channel
 * GGX FV LUT : x = LdotH, y = BRDF alpha
 * GGX D LUT  : x = NdotH, y = BRDF alpha
 */
Pixels GenerateFVD(unsigned a_Width, unsigned a_Height);
}