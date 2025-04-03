#pragma once

#include <glm/vec3.hpp>

namespace MSG {
struct VolumetricFogSettings {
    float minDistance = 1.f; // the distance at which the volumetric fog should start rendering
    float maxDistance = 1000.f; // the max distance the volumetric fog will be displayed
    float depthExp    = 2.f; // the depth exponent used to render volumetric fog
};
struct FogSettings {
    glm::vec3 globalScattering   = { 1, 1, 1 };
    glm::vec3 globalEmissive     = { 0, 0, 0 };
    float globalPhaseG           = 0.f;
    float globalExtinction       = 0.1f;
    glm::vec3 noiseDensityOffset = { 0, 0, 0 };
    float noiseDensityScale      = 0.5f;
    float noiseDensityIntensity  = 1.0f;
    VolumetricFogSettings volumetricFog;
};
}