#include <MSG/Buffer/View.hpp>
#include <MSG/Cubemap.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/BRDFIntegration.hpp>
#include <MSG/Tools/Halton.hpp>
#include <MSG/Tools/SphericalHarmonics.hpp>
#include <MSG/Tools/ThreadPool.hpp>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

namespace MSG {
template <unsigned Size>
glm::vec2 Halton23(const unsigned& a_Index)
{
    constexpr auto halton2 = Tools::Halton<2>::Sequence<Size>();
    constexpr auto halton3 = Tools::Halton<3>::Sequence<Size>();
    const auto rIndex      = a_Index % Size;
    return { halton2[rIndex], halton3[rIndex] };
}

float DistributionGGX(float NdotH, float alpha)
{
    float alpha2 = alpha * alpha;
    float d      = NdotH * NdotH * (alpha2 - 1.f) + 1.f;
    return alpha2 / (M_PIf * d * d);
}

template <unsigned Samples>
PixelColor SampleGGX(
    const TextureSamplerCube& a_Src,
    const glm::vec3& a_SampleDir,
    const float& a_Roughness)
{
    auto& res             = a_Src.texture->GetSize().x;
    glm::vec3 N           = a_SampleDir;
    glm::vec3 V           = a_SampleDir;
    PixelColor finalColor = { 0.f, 0.f, 0.f, 0.f };
    for (auto i = 0u; i < Samples; ++i) {
        const auto halton23 = Halton23<Samples>(i);
        const auto H        = Tools::BRDFIntegration::ImportanceSampleGGX(halton23, N, a_Roughness);
        const auto L        = 2 * glm::dot(V, H) * H - V;
        const auto NoL      = glm::max(glm::dot(N, L), 0.f);
        if (NoL <= 0)
            continue;
        const auto NdotH = glm::max(glm::dot(N, H), 0.f);
        const auto HdotV = glm::max(glm::dot(H, V), 0.f);
        float D          = DistributionGGX(NdotH, a_Roughness);
        float pdf        = (D * NdotH / (4.f * HdotV)) + 0.0001f;
        float saTexel    = 4.f * M_PIf / float(6 * res * res);
        float saSample   = 1.f / (float(Samples) * pdf + 0.0001f);
        float mipLevel   = a_Roughness == 0.f ? 0.f : 0.5f * log2(saSample / saTexel);
        const auto color = a_Src.Sample(L, mipLevel);
        finalColor += color * NoL;
    }
    // we're bound to have at least one sample
    return { glm::vec3(finalColor) / finalColor.w, 1.f };
}

void GenerateLevel(
    Tools::ThreadPool& a_ThreadPool,
    const TextureSamplerCube& a_Src,
    Cubemap& a_Level,
    const float& a_Roughness)
{
    for (auto z = 0u; z < 6; ++z) {
        a_ThreadPool.PushCommand([z, a_Src, a_Level, a_Roughness]() mutable {
            for (auto y = 0u; y < a_Level.GetSize().y; ++y) {
                for (auto x = 0u; x < a_Level.GetSize().x; ++x) {
                    const auto uv        = glm::vec2(x, y) / glm::vec2(a_Level.GetSize());
                    const auto sampleDir = Cubemap::UVToXYZ(CubemapSide(z), uv);
                    const auto color     = SampleGGX<512>(a_Src, sampleDir, a_Roughness);
                    a_Level.Store({ x, y, z }, color);
                }
            }
        },
            false);
    }
}

Texture GenerateIBlSpecular(
    const TextureSamplerCube& a_Src,
    const glm::ivec2& a_Size)
{
    Tools::ThreadPool threadPool;
    const auto pixelDesc = a_Src.texture->GetPixelDescriptor();
    auto mipsCount       = 0;
    auto specular        = Texture(TextureType::TextureCubemap);
    specular.SetPixelDescriptor(pixelDesc);
    specular.SetSize({ a_Size, 1 });
    std::vector<std::shared_ptr<Image>> mipMaps;
    for (auto size = a_Size; size.x >= 16 && size.y >= 16; size /= 2.f) {
        auto level = std::make_shared<Cubemap>(pixelDesc, size.x, size.y);
        level->Allocate();
        mipMaps.emplace_back(level);
        mipsCount++;
    }
    specular = mipMaps;
    for (auto i = 0; i < mipsCount; ++i) {
        const auto roughness = float(i) / float(mipsCount);
        auto& level          = *std::static_pointer_cast<Cubemap>(specular[i]);
        GenerateLevel(threadPool, a_Src, level, roughness);
    }
    return specular;
}

auto CreateTexAndGenMips(const std::shared_ptr<Cubemap>& a_Cubemap)
{
    auto texture = std::make_shared<Texture>(TextureType::TextureCubemap, a_Cubemap);
    texture->GenerateMipmaps();
    return texture;
}

LightIBL::LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Texture>& a_Skybox)
{
    TextureSamplerCube src {
        a_Skybox,
        std::make_shared<Sampler>()
    };
    specular.sampler = src.sampler;
    specular.texture = std::make_shared<Texture>(GenerateIBlSpecular(src, a_Size));
    specular.sampler->SetMagFilter(Sampler::Filter::LinearMipmapLinear);
    irradianceCoefficients = Tools::SphericalHarmonics<256>().Eval<glm::vec3>([sampler = specular](const auto& sampleDir) {
        return sampler.Sample(sampleDir.vec, sampler.texture->size() - 1);
    });
}

LightIBL::LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Cubemap>& a_Skybox)
    : LightIBL(a_Size, CreateTexAndGenMips(a_Skybox))
{
}
}
