#pragma once

#include <glm/fwd.hpp>

namespace Msg::Renderer {
float WorleyPerlinNoise(const glm::vec3& a_UVW, const float& a_Freq);
}