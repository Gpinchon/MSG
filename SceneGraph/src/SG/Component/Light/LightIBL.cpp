#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Core/Image/Cubemap.hpp>

namespace TabGraph::SG::Component {

LightIBL::LightIBL(const glm::ivec2& a_Size, const Cubemap& a_Skybox)
{
    specular = std::make_shared<Cubemap>(Pixel::SizedFormat::Uint8_NormalizedRGB, a_Size.x, a_Size.y);
    for (auto z = 0u; z < 6; ++z) {
        for (auto y = 0u; y < a_Size.y; ++y) {
            for (auto x = 0u; x < a_Size.x; ++x) {
                const auto UV          = glm::vec2(x, y) / glm::vec2(specular->at(z).GetSize());
                const auto samplingDir = Cubemap::UVToXYZ(CubemapSide(z), UV);
                const auto sampleColor = a_Skybox.LoadNorm(samplingDir, ImageFilter::Bilinear);
                specular->Store({ x, y, z }, sampleColor);
            }
        }
    }
}
}