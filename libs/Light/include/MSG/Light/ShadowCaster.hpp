#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg {
enum class LightShadowPrecision {
    High,
    Medium,
    Low
};

/**
 * @brief purely indicative, Renderer is free to ignore
 * @brief normalBias describes the maximum offset of shadow sampling position
 * in world units when surface is at 90 degrees relative to light dir
 */
struct LightShadowSettings {
    bool castShadow                = false;
    LightShadowPrecision precision = LightShadowPrecision::High;
    float bias                     = 0.0025f; // the base amount of bias to apply to shadow maps
    float normalBias               = 0.1f; // the amount of extra bias to be applied to 90 degrees normals in texels
    float blurRadius               = 1.f; // blur radius in texels
    uint16_t resolution            = 512;
    uint8_t cascadeCount           = 1; // 1 means no cascades
};
}
