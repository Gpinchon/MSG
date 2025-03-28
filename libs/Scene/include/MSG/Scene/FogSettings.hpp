#pragma once

#include <glm/vec3.hpp>

namespace MSG {
struct FogSettings {
    glm::vec3 noiseDensityOffset = { 0, 0, 0 };
    float noiseDensityScale      = 0.5f;
    float noiseDensityIntensity  = 1.0f;
    float noiseDepthMultiplier   = 0.025f;
    float multiplier             = 1.0f;
    float attenuationExp         = 1.f;
};
}