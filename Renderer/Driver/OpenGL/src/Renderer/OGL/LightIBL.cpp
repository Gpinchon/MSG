#include <Renderer/OGL/LightIBL.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Image/Pixel.hpp>
#include <Tools/Halton.hpp>

#include <Functions.glsl>

#include <algorithm>
#include <cmath>
#include <cstdint>

#include <glm/glm.hpp>

namespace TabGraph::Renderer {
template <unsigned Size>
glm::vec2 Halton23(const unsigned& a_Index)
{
    constexpr auto halton2 = Tools::Halton<2>::Sequence<Size>();
    constexpr auto halton3 = Tools::Halton<3>::Sequence<Size>();
    const auto rIndex      = a_Index % Size;
    return { halton2[rIndex], halton3[rIndex] };
}

glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, float a_Alpha, glm::vec3 N)
{
    float Phi      = 2 * M_PI * Xi.x;
    float CosTheta = sqrt((1.f - Xi.y) / (1 + (a_Alpha * a_Alpha - 1) * Xi.y));
    float SinTheta = sqrt(1.f - CosTheta * CosTheta);
    glm::vec3 H    = {
        SinTheta * cos(Phi),
        SinTheta * sin(Phi),
        CosTheta
    };

    glm::vec3 UpVector = abs(N.z) < 0.999 ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0);
    glm::vec3 TangentX = glm::normalize(glm::cross(UpVector, N));
    glm::vec3 TangentY = glm::cross(N, TangentX);

    return glm::normalize(TangentX * H.x + TangentY * H.y + N * H.z);
}

template <unsigned Samples>
SG::Pixel::Color SampleGGX(const SG::Cubemap& a_Src, const glm::vec3& a_SampleDir, const float& a_Alpha)
{
    constexpr auto SampleRcp    = 1 / float(Samples);
    glm::vec3 N                 = a_SampleDir;
    glm::vec3 V                 = a_SampleDir;
    SG::Pixel::Color finalColor = { 0.f, 0.f, 0.f, 0.f };
    for (auto i = 0u; i < Samples; ++i) {
        const auto halton23 = Halton23<Samples>(i);
        const auto H        = ImportanceSampleGGX(halton23, a_Alpha, N);
        const auto L        = 2 * glm::dot(V, H) * H - V;
        const auto NoL      = glm::max(glm::dot(N, L), 0.f);
        const auto color    = a_Src.LoadNorm(L);
        finalColor += color * NoL;
    }
    return finalColor * SampleRcp;
}

void GenerateLevel(const SG::Cubemap& a_Src, SG::Cubemap& a_Dst, const float& a_Alpha)
{
    glm::vec2 baseSize = a_Dst.GetSize();
    for (auto z = 0; z < 6; ++z) {
        for (auto y = 0; y < a_Dst.GetSize().y; ++y) {
            for (auto x = 0; x < a_Dst.GetSize().x; ++x) {
                const auto uv        = glm::vec2(x, y) / baseSize;
                const auto sampleDir = SG::Cubemap::UVToXYZ(SG::CubemapSide(z), uv);
                const auto color     = SampleGGX<64>(a_Src, sampleDir, a_Alpha);
                a_Dst.Store({ x, y, z }, color);
            }
        }
    }
}

std::vector<SG::Cubemap> GenerateIBlSpecularMips(
    const SG::Cubemap& a_Base)
{
    std::vector<SG::Cubemap> cubemaps;
    const auto baseSize  = glm::ivec2(a_Base.GetSize());
    const auto pixelSize = a_Base.GetPixelDescription().GetSize();
    const auto mipsCount = MIPMAPNBR(baseSize);
    unsigned dataSize    = 0;
    for (auto i = 1; i < mipsCount; ++i) {
        const auto size        = glm::max(baseSize / int(pow(2, i)), glm::ivec2(1));
        const auto mipDataSize = pixelSize * size.x * size.y * 6;
        dataSize += mipDataSize;
    }
    auto buffer         = std::make_shared<SG::Buffer>(dataSize);
    unsigned dataOffset = 0;
    auto prevLevel      = &a_Base;
    cubemaps.reserve(mipsCount);
    for (auto i = 1; i < mipsCount; ++i) {
        const auto size        = glm::max(baseSize / int(pow(2, i)), glm::ivec2(1));
        const auto mipDataSize = pixelSize * size.x * size.y * 6;
        const auto alpha       = (i + 1) / float(mipsCount);
        const auto bufferView  = std::make_shared<SG::BufferView>(buffer, dataOffset, mipDataSize);
        auto& level            = cubemaps.emplace_back(a_Base.GetPixelDescription(), size.x, size.y, bufferView);
        GenerateLevel(*prevLevel, level, alpha);
        dataOffset += mipDataSize;
        prevLevel = &level;
    }
    return cubemaps;
}
}
