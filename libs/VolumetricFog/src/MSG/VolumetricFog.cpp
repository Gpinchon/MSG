#include <MSG/VolumetricFog.hpp>

MSG::VolumetricFog::VolumetricFog(const glm::vec3& a_Color, const float& a_Intensity)
    : _Grid({ .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedRGBA })
{
    _Grid.Allocate();
    _Grid.Fill(glm::vec4(a_Color, a_Intensity));
}