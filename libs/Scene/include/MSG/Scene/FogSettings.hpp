#pragma once

#include <glm/vec3.hpp>

namespace MSG {
/**
 * @brief It is highly recommended to customize these settings depending on your scene
 *
 */
struct VolumetricFogSettings {
    float minDistance            = 0.5f; // the distance at which the volumetric fog should start rendering
    float maxDistance            = 100.f; // the max distance the volumetric fog will be displayed
    float depthExp               = 5.f; // the depth exponent used to render volumetric fog
    glm::vec3 noiseDensityOffset = { 0, 0, 0 };
    float noiseDensityScale      = 0.5f;
    float noiseDensityIntensity  = 1.0f;
    float noiseDensityMaxDist    = 5.f; // the maximum depth at which density noise will be applied before being progressively reduced
};
struct FogSettings {
    glm::vec3 globalScattering = { 1, 1, 1 };
    glm::vec3 globalEmissive   = { 0, 0, 0 };
    float globalPhaseG         = 0.f;
    float globalExtinction     = 0.025f;
    bool fogBackground         = true;
    VolumetricFogSettings volumetricFog;
};
}