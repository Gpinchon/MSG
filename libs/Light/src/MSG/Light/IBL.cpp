#include <MSG/Buffer/View.hpp>
#include <MSG/Image/Cubemap.hpp>
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
    const Texture& a_Src,
    const SamplerCube& a_Sampler,
    const glm::vec3& a_SampleDir,
    const float& a_Roughness)
{
    auto& res             = a_Src.GetSize().x;
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
        const auto color = a_Sampler.Sample(a_Src, L, mipLevel);
        finalColor += color * NoL;
    }
    // we're bound to have at least one sample
    return { glm::vec3(finalColor) / finalColor.w, 1.f };
}

void GenerateLevel(
    Tools::ThreadPool& a_ThreadPool,
    const Texture& a_Src,
    const SamplerCube& a_Sampler,
    Image& a_Level,
    const float& a_Roughness)
{
    for (auto z = 0u; z < 6; ++z) {
        a_ThreadPool.PushCommand([z, a_Src, a_Sampler, a_Level, a_Roughness]() mutable {
            for (auto y = 0u; y < a_Level.GetSize().y; ++y) {
                for (auto x = 0u; x < a_Level.GetSize().x; ++x) {
                    const auto uv        = glm::vec2(x, y) / glm::vec2(a_Level.GetSize());
                    const auto sampleDir = CubemapUVWToSampleVec(uv, CubemapSide(z));
                    const auto color     = SampleGGX<512>(a_Src, a_Sampler, sampleDir, a_Roughness);
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
    Tools::ThreadPool threadPool;
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
    for (auto i = 0; i < mipsCount; ++i) {
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
        [sampler = SamplerCube { *specular.sampler }, texture = *specular.texture](const auto& sampleDir) {
            return sampler.Sample(texture, sampleDir.vec, texture.size() - 1);
        });
}

LightIBL::LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Image>& a_Skybox)
    : LightIBL(a_Size, CreateTexAndGenMips(a_Skybox))
{
}
}
