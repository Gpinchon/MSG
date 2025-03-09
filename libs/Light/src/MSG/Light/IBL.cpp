#include <MSG/BRDF.hpp>
#include <MSG/Buffer/View.hpp>
#include <MSG/Image/Cubemap.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/ThreadPool.hpp>
#include <MSG/Tools/Halton.hpp>
#include <MSG/Tools/SphericalHarmonics.hpp>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

namespace MSG {
#ifndef NDEBUG
constexpr uint16_t SamplesCount = 512;
#else
constexpr uint16_t SamplesCount = 2048;
#endif

float DistributionGGX(float a_NoH, float a_Alpha)
{
    float alpha2 = a_Alpha * a_Alpha;
    float d      = a_NoH * a_NoH * (alpha2 - 1.f) + 1.f;
    return alpha2 / (M_PIf * d * d);
}

template <unsigned Samples>
PixelColor SampleGGX(
    const Texture& a_Src,
    const SamplerCube& a_Sampler,
    const glm::vec3& a_SampleDir,
    const float& a_Roughness)
{
    const auto& res       = a_Src.GetSize().x;
    glm::vec3 N           = a_SampleDir;
    glm::vec3 V           = a_SampleDir;
    PixelColor finalColor = { 0.f, 0.f, 0.f, 0.f };
    for (auto i = 0u; i < Samples; ++i) {
        const auto Xi  = Tools::Halton23<Samples>(i);
        const auto H   = BRDF::ImportanceSampleGGX(Xi, N, a_Roughness);
        const auto VoH = glm::dot(V, H);
        const auto L   = 2.f * VoH * H - V;
        const auto NoL = glm::clamp(glm::dot(N, L), 0.f, 1.f);
        if (NoL == 0)
            continue;
        const auto NoH       = glm::clamp(VoH, 0.f, 1.f);
        const float pdf      = DistributionGGX(NoH, a_Roughness) / 4.f + 0.001f;
        const float oS       = 1.f / float(Samples * pdf);
        const float oP       = 4.f * M_PIf / (6.f * res * res);
        const float mipLevel = std::max(0.5f * float(log2(oS / oP)), 0.f);
        auto color           = a_Sampler.Sample(a_Src, L, mipLevel);
        finalColor += color * NoL;
    }
    // finalColor.w is the addition of every NoL since the env map is opaque
    return finalColor / finalColor.w;
}

void GenerateLevel(
    ThreadPool& a_ThreadPool,
    const Texture& a_Src,
    const SamplerCube& a_Sampler,
    Image& a_Level,
    const float& a_Roughness)
{
    for (auto z = 0u; z < 6; ++z) {
        a_ThreadPool.PushCommand([z, a_Src, a_Sampler, a_Level, a_Roughness]() mutable {
            for (auto y = 0u; y < a_Level.GetSize().y; ++y) {
                const float v = y / float(a_Level.GetSize().y);
                for (auto x = 0u; x < a_Level.GetSize().x; ++x) {
                    const float u        = x / float(a_Level.GetSize().x);
                    const auto sampleDir = CubemapUVWToSampleVec({ u, v }, CubemapSide(z));
                    const auto color     = SampleGGX<SamplesCount>(a_Src, a_Sampler, sampleDir, a_Roughness);
                    a_Level.Store({ x, y, z }, color);
                }
            }
        },
            false);
    }
}

Texture GenerateIBlSpecular(
    const Texture& a_Src,
    const SamplerCube& a_Sampler,
    const glm::ivec2& a_Size)
{
    ThreadPool threadPool;
    const auto pixelDesc = a_Src.GetPixelDescriptor();
    auto mipsCount       = 0;
    auto specular        = Texture(TextureType::TextureCubemap);
    specular.SetPixelDescriptor(pixelDesc);
    specular.SetSize({ a_Size, 1 });
    std::vector<std::shared_ptr<Image>> mipMaps;
    for (auto size = a_Size; size.x >= 16 && size.y >= 16; size /= 2.f) {
        auto level = std::make_shared<Image>(pixelDesc, size.x, size.y, 6);
        level->Allocate();
        mipMaps.emplace_back(level);
        mipsCount++;
    }
    specular = mipMaps;
    // First level is just the original environment
    a_Src.front()->Blit(*specular.front(), { 0, 0, 0 }, a_Src.GetSize());
    for (auto i = 1; i < mipsCount; ++i) {
        const auto roughness = float(i) / float(mipsCount);
        auto& level          = *std::static_pointer_cast<Image>(specular[i]);
        GenerateLevel(threadPool, a_Src, a_Sampler, level, roughness);
    }
    return specular;
}

auto CreateTexAndGenMips(const std::shared_ptr<Image>& a_Image)
{
    auto texture = std::make_shared<Texture>(TextureType::TextureCubemap, a_Image);
    texture->GenerateMipmaps();
    return texture;
}

LightIBL::LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Texture>& a_Skybox)
{
    auto sampler = std::make_shared<Sampler>();
    sampler->SetMagFilter(SamplerFilter::LinearMipmapLinear);
    specular.sampler       = sampler;
    specular.texture       = std::make_shared<Texture>(GenerateIBlSpecular(*a_Skybox, *sampler, a_Size));
    irradianceCoefficients = Tools::SphericalHarmonics<256>().Eval<glm::vec3>(
        [sampler = SamplerCube { *sampler }, &texture = *specular.texture](const auto& sampleDir) {
            return sampler.Sample(texture, sampleDir.vec, texture.size() - 1);
        });
}

LightIBL::LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Image>& a_Skybox)
    : LightIBL(a_Size, CreateTexAndGenMips(a_Skybox))
{
}
}
