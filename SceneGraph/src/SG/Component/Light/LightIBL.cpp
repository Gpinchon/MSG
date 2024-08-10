#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Core/Texture/TextureSampler.hpp>
#include <Tools/BRDFIntegration.hpp>
#include <Tools/Halton.hpp>
#include <Tools/ThreadPool.hpp>

#include <glm/geometric.hpp>
#include <glm/vec2.hpp>

namespace TabGraph::SG::Component {
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
    float d      = NdotH * NdotH * (alpha2 - 1.0) + 1.0;
    return alpha2 / (M_PI * d * d);
}

template <unsigned Samples>
SG::Pixel::Color SampleGGX(const SG::Texture& a_Src, const glm::vec3& a_SampleDir, const float& a_Roughness)
{
    glm::vec3 N                 = a_SampleDir;
    glm::vec3 V                 = a_SampleDir;
    SG::Pixel::Color finalColor = { 0.f, 0.f, 0.f, 0.f };
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
        float pdf        = (D * NdotH / (4.0 * HdotV)) + 0.0001;
        const auto res   = a_Src.GetSize().x;
        float saTexel    = 4.0 * M_PI / (6.0 * res * res);
        float saSample   = 1.0 / (float(Samples) * pdf + 0.0001);
        float mipLevel   = a_Roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
        const auto color = a_Src[mipLevel]->LoadNorm(L);
        finalColor += color * NoL;
    }
    // we're bound to have at least one sample
    return { glm::vec3(finalColor) / finalColor.w, 1.f };
}

void GenerateLevel(const SG::Texture& a_Src, SG::Cubemap& a_Level, const float& a_Roughness)
{
    for (auto z = 0; z < 6; ++z) {
        for (auto y = 0; y < a_Level.GetSize().y; ++y) {
            for (auto x = 0; x < a_Level.GetSize().x; ++x) {
                const auto uv        = glm::vec2(x, y) / glm::vec2(a_Level.GetSize());
                const auto sampleDir = SG::Cubemap::UVToXYZ(SG::CubemapSide(z), uv);
                const auto color     = SampleGGX<512>(a_Src, sampleDir, a_Roughness);
                a_Level.Store({ x, y, z }, color);
            }
        }
    }
}

SG::Texture GenerateIBlSpecular(
    const std::shared_ptr<Cubemap>& a_Base,
    const glm::ivec2& a_Size)
{
    const auto pixelDesc = a_Base->GetPixelDescription();
    auto mipsCount       = 0;
    auto specular        = SG::Texture(TextureType::TextureCubemap);
    specular.SetPixelDescription(pixelDesc);
    specular.SetSize({ a_Size, 1 });
    std::vector<std::shared_ptr<SG::Image>> mipMaps;
    for (auto size = a_Size; size.x >= 16 && size.y >= 16; size /= 2.f) {
        auto level = std::make_shared<SG::Cubemap>(pixelDesc, size.x, size.y);
        level->Allocate();
        mipMaps.emplace_back(level);
        mipsCount++;
    }
    specular.SetLevels(mipMaps);
    auto base = SG::Texture(TextureType::TextureCubemap, a_Base);
    base.GenerateMipmaps();
    Tools::ThreadPool threadPool;
    for (auto i = 0; i < mipsCount; ++i) {
        const auto roughness = float(i) / float(mipsCount);
        auto& level          = *std::static_pointer_cast<SG::Cubemap>(specular[i]);
        threadPool.PushCommand([base, level, roughness]() mutable {
            GenerateLevel(base, level, roughness);
        },
            false);
    }
    threadPool.Wait();
    return specular;
}

LightIBL::LightIBL(const glm::ivec2& a_Size, const std::shared_ptr<Cubemap>& a_Skybox)
{
    specular.sampler = std::make_shared<SG::Sampler>();
    specular.texture = std::make_shared<SG::Texture>(GenerateIBlSpecular(a_Skybox, a_Size));
    specular.sampler->SetMagFilter(SG::Sampler::Filter::LinearMipmapLinear);
}
}