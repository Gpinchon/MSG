#include <MSG/FogArea.hpp>

MSG::FogArea::FogArea(const glm::vec3& a_Scattering, const float& a_Extinction)
    : _ScatteringExtinction({ .pixelDesc = MSG::PixelSizedFormat::Uint8_NormalizedRGBA })
{
    _ScatteringExtinction.Allocate();
    _ScatteringExtinction.Fill(glm::vec4(a_Scattering, a_Extinction));
}