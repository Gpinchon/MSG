#pragma once

#include <glm/fwd.hpp>

namespace MSG::Renderer {
float WorleyPerlinNoise(const glm::vec3& a_UVW, const float& a_Freq);
}