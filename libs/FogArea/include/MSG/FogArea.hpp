#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Image.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
enum class FogAreaMode {
    Add,
    Substract
};
class FogArea {
    /** @brief 3D image storing the fog's scattering and opacity, by default this is only 1 pixel */
    PROPERTY(MSG::Image, ScatteringExtinction, { .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedRGBA });
    /**
     * @brief G phase function describes how much forward (g<0) or backward (g > 0) light scatters around.
     * Valid range : [-1, 1]
     */
    PROPERTY(float, PhaseG, 0.75);
    PROPERTY(FogAreaMode, FogAreaMode, FogAreaMode::Add);
    PROPERTY(glm::vec3, HalfSize, std::numeric_limits<float>::infinity());

public:
    FogArea(const glm::vec3& a_Scattering = glm::vec3(1.f), const float& a_Extinction = 0.01f);
};
}
