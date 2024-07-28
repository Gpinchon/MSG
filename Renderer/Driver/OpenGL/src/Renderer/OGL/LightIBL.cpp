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
glm::vec2 Halton23(const unsigned& a_Index)
{
    const auto rIndex      = a_Index % 256;
    constexpr auto halton2 = Tools::Halton<2>::Sequence<256>();
    constexpr auto halton3 = Tools::Halton<3>::Sequence<256>();
    return { halton2[rIndex], halton3[rIndex] };
}

float specularD(float roughness, float NoH)
{
    float NoH2 = NoH * NoH;
    float r2   = roughness * roughness;
    return r2 / pow(NoH2 * (r2 - 1.0) + 1.0, 2.0);
}

glm::vec3 ImportanceSampleGGX(glm::vec2 Xi, float a_Alpha, glm::vec3 N)
{
    float Phi      = 2 * M_PI * Xi.x;
    float CosTheta = sqrt((1 - Xi.y) / (1 + (a_Alpha * a_Alpha - 1) * Xi.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);
    glm::vec3 H    = {
        SinTheta * cos(Phi),
        SinTheta * sin(Phi),
        CosTheta
    };

    glm::vec3 UpVector = abs(N.z) < 0.999 ? glm::vec3(0, 0, 1) : glm::vec3(1, 0, 0);
    glm::vec3 TangentX = glm::normalize(glm::cross(UpVector, N));
    glm::vec3 TangentY = glm::cross(N, TangentX);

    return TangentX * H.x + TangentY * H.y + N * H.z;
}

template <unsigned Samples>
SG::Pixel::Color SampleGGX(const SG::Cubemap& a_Src, const glm::vec3& a_SampleDir, const float& a_Roughness)
{
    glm::vec3 N                 = a_SampleDir;
    glm::vec3 V                 = a_SampleDir;
    constexpr auto SampleRcp    = 1 / float(Samples);
    SG::Pixel::Color finalColor = { 0.f, 0.f, 0.f, 0.f };
    for (auto i = 0u; i < Samples; ++i) {
        const auto halton23 = Halton23(i);
        const auto H        = ImportanceSampleGGX(halton23, a_Roughness, N);
        const auto L        = 2 * glm::dot(V, H) * H - V;
        const auto NoL      = glm::max(glm::dot(N, L), 0.f);
        const auto color    = a_Src.LoadNorm(L);
        finalColor += color * NoL;
    }
    return finalColor * SampleRcp;
}

void GenerateLevel(const SG::Cubemap& a_Src, SG::Cubemap& a_Dst, const float& a_Roughness)
{
    glm::vec2 baseSize = a_Src.GetSize();
    for (auto z = 0; z < 6; ++z) {
        for (auto y = 0; y < a_Dst.GetSize().y; ++y) {
            for (auto x = 0; x < a_Dst.GetSize().x; ++x) {
                const auto uv        = glm::vec2(x, y) / baseSize;
                const auto sampleDir = SG::Cubemap::UVToXYZ(SG::CubemapSide(z), uv);
                const auto color     = SampleGGX<16>(a_Src, sampleDir, a_Roughness);
                a_Dst.Store({ x, y, z }, color);
            }
        }
    }
}

std::vector<SG::Cubemap> GenerateIBlSpecularMips(
    const unsigned& a_Width,
    const unsigned& a_Height,
    const SG::Cubemap& a_Base)
{
    std::vector<SG::Cubemap> cubemaps;
    const auto baseSize  = glm::ivec2(a_Width, a_Height);
    const auto pixelSize = SG::Pixel::Description { SG::Pixel::SizedFormat::Uint8_NormalizedRGB }.GetSize();
    const auto mipsCount = MIPMAPNBR(baseSize);
    unsigned dataSize    = 0;
    for (auto i = 0; i < mipsCount; ++i) {
        const auto size        = baseSize / (2 * i);
        const auto mipDataSize = pixelSize * size.x * size.y * 6;
        dataSize += mipDataSize;
    }
    SG::Buffer buffer(dataSize);
    unsigned dataOffset = 0;
    for (auto i = 0; i < mipsCount; ++i) {
        const auto size        = baseSize / (2 * i);
        const auto mipDataSize = pixelSize * size.x * size.y * 6;
        const auto roughness   = i / float(mipsCount);
        const auto bufferView  = std::make_shared<SG::BufferView>(&buffer, dataOffset, mipDataSize);
        auto& level            = cubemaps.emplace_back(SG::Pixel::SizedFormat::Uint8_RGB, size.x, size.y, bufferView);
        GenerateLevel(a_Base, level, roughness);
        dataOffset += mipDataSize;
    }
    return cubemaps;
}
}
