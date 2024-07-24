#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/Base.hpp>
#include <SG/Core/Material/Extension/MetallicRoughness.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>

#include <Material.glsl>

#include <GL/glew.h>
#include <iostream>

namespace TabGraph::Renderer {
std::shared_ptr<TabGraph::SG::Texture> CreateSGTexture(
    const std::shared_ptr<SG::TextureSampler>& a_Sampler,
    const glm::ivec3& a_Size,
    const SG::Pixel::Description& a_PixelDesc)
{
    auto texture    = std::make_shared<SG::Texture>(SG::Texture::Type::Texture2D);
    auto bufferView = std::make_shared<SG::BufferView>(0, a_Size.x * a_Size.y * a_Size.z * a_PixelDesc.GetSize());
    auto image      = std::make_shared<SG::Image>(SG::ImageType::Image2D, a_PixelDesc, a_Size, bufferView);
    texture->SetImage(image);
    texture->SetSampler(a_Sampler);
    return texture;
}

auto GetDefaultSampler()
{
    static std::shared_ptr<SG::TextureSampler> sampler;
    if (sampler == nullptr) {
        sampler = std::make_shared<SG::TextureSampler>();
        sampler->SetMinFilter(SG::TextureSampler::Filter::Nearest);
        sampler->SetMagFilter(SG::TextureSampler::Filter::Nearest);
    }
    return sampler;
}

auto GetDefaultDiffuse()
{
    static std::shared_ptr<SG::Texture> texture;
    if (texture != nullptr)
        return texture;
    glm::ivec3 imageSize = { 2, 2, 1 };
    texture              = CreateSGTexture(GetDefaultSampler(), imageSize, SG::Pixel::SizedFormat::Uint8_NormalizedRGBA);
    auto const& image    = texture->GetImage();
    for (auto z = 0u; z < imageSize.z; ++z) {
        for (auto y = 0u; y < imageSize.y; ++y) {
            for (auto x = 0u; x < imageSize.x; ++x) {
                image->SetColor({ x, y, z }, { x, y, z, 1 });
            }
        }
    }
    return texture;
}

auto GetDefaultSpecGloss()
{
    static std::shared_ptr<SG::Texture> texture;
    if (texture != nullptr)
        return texture;
    glm::ivec3 imageSize = { 2, 2, 1 };
    texture              = CreateSGTexture(GetDefaultSampler(), imageSize, SG::Pixel::SizedFormat::Uint8_NormalizedRGBA);
    auto const& image    = texture->GetImage();
    for (auto z = 0u; z < imageSize.z; ++z) {
        for (auto y = 0u; y < imageSize.y; ++y) {
            for (auto x = 0u; x < imageSize.x; ++x) {
                auto total = x + y + z;
                image->SetColor({ x, y, z }, { 0, 0, 0, (total % 2 == 0) });
            }
        }
    }
    return texture;
}

auto GetDefaultNormal()
{
    static std::shared_ptr<SG::Texture> texture;
    if (texture != nullptr)
        return texture;
    glm::ivec3 imageSize = { 1, 1, 1 };
    texture              = CreateSGTexture(GetDefaultSampler(), imageSize, SG::Pixel::SizedFormat::Uint8_NormalizedRGB);
    auto const& image    = texture->GetImage();
    for (auto z = 0u; z < imageSize.z; ++z) {
        for (auto y = 0u; y < imageSize.y; ++y) {
            for (auto x = 0u; x < imageSize.x; ++x) {
                image->SetColor({ x, y, z }, { 0.5, 0.5, 1.0, 1.0 });
            }
        }
    }
    return texture;
}

void Material::Set(
    Renderer::Impl& a_Renderer,
    const SG::Material& a_SGMaterial)
{
    if (a_SGMaterial.HasExtension<SG::BaseExtension>())
        _LoadBaseExtension(a_Renderer,
            a_SGMaterial.GetExtension<SG::BaseExtension>());
    else
        _LoadBaseExtension(a_Renderer, {});
    if (a_SGMaterial.HasExtension<SG::SpecularGlossinessExtension>())
        _LoadSpecGlossExtension(a_Renderer, a_SGMaterial.GetExtension<SG::SpecularGlossinessExtension>());
    else if (a_SGMaterial.HasExtension<SG::MetallicRoughnessExtension>())
        _LoadMetRoughExtension(a_Renderer, a_SGMaterial.GetExtension<SG::MetallicRoughnessExtension>());
    else
        _LoadSpecGlossExtension(a_Renderer, {});
}

void FillTextureInfo(
    GLSL::TextureInfo& a_Info,
    const SG::TextureInfo& a_SGTextureInfo)
{
    a_Info.texCoord           = a_SGTextureInfo.texCoord;
    a_Info.transform.offset   = a_SGTextureInfo.transform.offset;
    a_Info.transform.rotation = a_SGTextureInfo.transform.rotation;
    a_Info.transform.scale    = a_SGTextureInfo.transform.scale;
}

void Material::_LoadBaseExtension(
    Renderer::Impl& a_Renderer,
    const SG::BaseExtension& a_Extension)
{
    auto UBOData    = GetData();
    auto& extension = UBOData.base;
    {
        auto& SGTexture        = a_Extension.normalTexture;
        auto& texture          = SGTexture.texture == nullptr ? GetDefaultNormal() : SGTexture.texture;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_BASE_NORMAL);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_BASE_NORMAL];
        textureSampler.sampler = a_Renderer.LoadSampler(texture->GetSampler().get());
        textureSampler.texture = a_Renderer.LoadTexture(texture->GetImage().get());
        extension.normalScale  = SGTexture.scale;
        FillTextureInfo(textureInfo, SGTexture);
    }
    SetData(UBOData);
}

void Material::_LoadSpecGlossExtension(
    Renderer::Impl& a_Renderer,
    const SG::SpecularGlossinessExtension& a_Extension)
{
    type                       = MATERIAL_TYPE_SPECULAR_GLOSSINESS;
    auto UBOData               = GetData();
    auto& extension            = UBOData.specularGlossiness;
    extension.diffuseFactor    = a_Extension.diffuseFactor;
    extension.specularFactor   = a_Extension.specularFactor;
    extension.glossinessFactor = a_Extension.glossinessFactor;
    {
        auto& SGTexture        = a_Extension.diffuseTexture;
        auto& texture          = SGTexture.texture == nullptr ? GetDefaultDiffuse() : SGTexture.texture;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_SPECGLOSS_DIFF);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_SPECGLOSS_DIFF];
        textureSampler.sampler = a_Renderer.LoadSampler(texture->GetSampler().get());
        textureSampler.texture = a_Renderer.LoadTexture(texture->GetImage().get());
        FillTextureInfo(textureInfo, SGTexture);
    }
    {
        auto& SGTexture        = a_Extension.specularGlossinessTexture;
        auto& texture          = SGTexture.texture == nullptr ? GetDefaultSpecGloss() : SGTexture.texture;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_SPECGLOSS_SG);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_SPECGLOSS_SG];
        textureSampler.sampler = a_Renderer.LoadSampler(texture->GetSampler().get());
        textureSampler.texture = a_Renderer.LoadTexture(texture->GetImage().get());
        FillTextureInfo(textureInfo, SGTexture);
    }
    SetData(UBOData);
}

void Material::_LoadMetRoughExtension(
    Renderer::Impl& a_Renderer,
    const SG::MetallicRoughnessExtension& a_Extension)
{
    type                      = MATERIAL_TYPE_METALLIC_ROUGHNESS;
    auto UBOData              = GetData();
    auto& extension           = UBOData.metallicRoughness;
    extension.colorFactor     = a_Extension.colorFactor;
    extension.metallicFactor  = a_Extension.metallicFactor;
    extension.roughnessFactor = a_Extension.roughnessFactor;
    SetData(UBOData);
}
}
