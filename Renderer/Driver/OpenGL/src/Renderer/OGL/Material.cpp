#include <Renderer/OGL/Material.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>

#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>
#include <SG/Core/Material.hpp>
#include <SG/Core/Material/Extension/Base.hpp>
#include <SG/Core/Material/Extension/MetallicRoughness.hpp>
#include <SG/Core/Material/Extension/SpecularGlossiness.hpp>
#include <SG/Core/Material/Extension/Unlit.hpp>
#include <SG/Core/Texture/Sampler.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <SG/Core/Texture/TextureSampler.hpp>

#include <Material.glsl>

#include <GL/glew.h>
#include <iostream>

namespace TabGraph::Renderer {
std::shared_ptr<TabGraph::SG::TextureSampler> CreateSGTextureSampler(
    const std::shared_ptr<SG::Sampler>& a_Sampler,
    const glm::uvec3& a_Size,
    const SG::Pixel::Description& a_PixelDesc)
{
    SG::TextureSampler textureSampler;
    auto image = std::make_shared<SG::Image2D>(a_PixelDesc, a_Size.x, a_Size.y);
    image->Allocate();
    textureSampler.texture = std::make_shared<SG::Texture>(SG::TextureType::Texture2D, image);
    textureSampler.sampler = a_Sampler;
    return std::make_shared<SG::TextureSampler>(textureSampler);
}

std::shared_ptr<SG::Texture> CreateSGTexture(
    const SG::Pixel::Description& a_PixelDesc,
    const glm::uvec3& a_Size)
{
    auto image = std::make_shared<SG::Image2D>(a_PixelDesc, a_Size.x, a_Size.y);
    image->Allocate();
    return std::make_shared<SG::Texture>(SG::TextureType::Texture2D, image);
}

auto GetDefaultSampler()
{
    static std::shared_ptr<SG::Sampler> sampler;
    if (sampler == nullptr) {
        sampler = std::make_shared<SG::Sampler>();
        sampler->SetMinFilter(SG::Sampler::Filter::Nearest);
        sampler->SetMagFilter(SG::Sampler::Filter::Nearest);
    }
    return sampler;
}

auto& GetWhiteTexture()
{
    static std::shared_ptr<SG::Texture> texture;
    if (texture != nullptr)
        return texture;
    texture = CreateSGTexture(SG::Pixel::SizedFormat::Uint8_NormalizedRGBA, { 1, 1, 1 });
    (*texture)[0]->Fill({ 1, 1, 1, 1 });
    return texture;
}

#define GetDefaultOcclusion         GetWhiteTexture
#define GetDefaultEmissive          GetWhiteTexture
#define GetDefaultSpecGloss         GetWhiteTexture
#define GetDefaultDiffuse           GetWhiteTexture
#define GetDefaultMetallicRoughness GetWhiteTexture
#define GetDefaultBaseColor         GetWhiteTexture

auto& GetDefaultNormal()
{
    static std::shared_ptr<SG::Texture> texture;
    if (texture != nullptr)
        return texture;
    texture = CreateSGTexture(SG::Pixel::SizedFormat::Uint8_NormalizedRGBA, { 1, 1, 1 });
    (*texture)[0]->Fill({ 0.5, 0.5, 1.0, 1.0 });
    return texture;
}

void Material::Set(
    Renderer::Impl& a_Renderer,
    const SG::Material& a_SGMaterial)
{
    if (a_SGMaterial.HasExtension<SG::BaseExtension>())
        _LoadBaseExtension(a_Renderer, a_SGMaterial.GetExtension<SG::BaseExtension>());
    else
        _LoadBaseExtension(a_Renderer, {});
    if (a_SGMaterial.HasExtension<SG::SpecularGlossinessExtension>())
        _LoadSpecGlossExtension(a_Renderer, a_SGMaterial.GetExtension<SG::SpecularGlossinessExtension>());
    else if (a_SGMaterial.HasExtension<SG::MetallicRoughnessExtension>())
        _LoadMetRoughExtension(a_Renderer, a_SGMaterial.GetExtension<SG::MetallicRoughnessExtension>());
    else
        _LoadSpecGlossExtension(a_Renderer, {});
    unlit = a_SGMaterial.HasExtension<SG::UnlitExtension>();
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
    auto UBOData             = GetData();
    auto& extension          = UBOData.base;
    extension.emissiveFactor = a_Extension.emissiveFactor;
    {
        auto& SGTextureInfo    = a_Extension.occlusionTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultOcclusion() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_BASE_OCCLUSION);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_BASE_OCCLUSION];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.LoadTexture(SGTexture.get());
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.emissiveTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultEmissive() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_BASE_EMISSIVE);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_BASE_EMISSIVE];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.LoadTexture(SGTexture.get());
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.normalTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultNormal() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_BASE_NORMAL);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_BASE_NORMAL];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.LoadTexture(SGTexture.get());
        extension.normalScale  = SGTextureInfo.scale;
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    if (a_Extension.alphaMode == SG::BaseExtension::AlphaMode::Opaque) {
        extension.alphaCutoff = 0;
        alphaMode             = MATERIAL_ALPHA_OPAQUE;
    } else if (a_Extension.alphaMode == SG::BaseExtension::AlphaMode::Blend) {
        extension.alphaCutoff = 1;
        alphaMode             = MATERIAL_ALPHA_BLEND;
    } else if (a_Extension.alphaMode == SG::BaseExtension::AlphaMode::Mask) {
        extension.alphaCutoff = a_Extension.alphaCutoff;
        alphaMode             = MATERIAL_ALPHA_CUTOFF;
    }
    doubleSided = a_Extension.doubleSided;
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
        auto& SGTextureInfo    = a_Extension.diffuseTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultDiffuse() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_SPECGLOSS_DIFF);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_SPECGLOSS_DIFF];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.LoadTexture(SGTexture.get());
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.specularGlossinessTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultSpecGloss() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_SPECGLOSS_SG);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_SPECGLOSS_SG];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.LoadTexture(SGTexture.get());
        FillTextureInfo(textureInfo, SGTextureInfo);
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
    {
        auto& SGTextureInfo    = a_Extension.colorTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultBaseColor() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_METROUGH_COL);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_METROUGH_COL];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.LoadTexture(SGTexture.get());
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.metallicRoughnessTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultMetallicRoughness() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_METROUGH_MR);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_METROUGH_MR];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.LoadTexture(SGTexture.get());
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    SetData(UBOData);
}
}
