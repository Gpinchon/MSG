#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Material.hpp>
#include <MSG/Material/Extension/Base.hpp>
#include <MSG/Material/Extension/MetallicRoughness.hpp>
#include <MSG/Material/Extension/SpecularGlossiness.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTexture.hpp>
#include <MSG/Renderer/OGL/Material.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/SparseTexture.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Texture/Sampler.hpp>

#include <Material.glsl>

#include <GL/glew.h>
#include <iostream>

namespace Msg::Renderer {
std::shared_ptr<Msg::TextureSampler> CreateSGTextureSampler(
    const std::shared_ptr<Sampler>& a_Sampler,
    const glm::uvec3& a_Size,
    const PixelDescriptor& a_PixelDesc)
{
    Msg::TextureSampler textureSampler;
    auto image = std::make_shared<Image>(
        ImageInfo {
            .width     = a_Size.x,
            .height    = a_Size.y,
            .pixelDesc = a_PixelDesc,
        });
    image->Allocate();
    textureSampler.texture = std::make_shared<Texture>(TextureType::Texture2D, image);
    textureSampler.sampler = a_Sampler;
    return std::make_shared<Msg::TextureSampler>(textureSampler);
}

std::shared_ptr<Texture> CreateSGTexture(
    const PixelDescriptor& a_PixelDesc,
    const glm::uvec3& a_Size)
{
    auto image = std::make_shared<Image>(
        ImageInfo {
            .width     = a_Size.x,
            .height    = a_Size.y,
            .pixelDesc = a_PixelDesc,
        });
    image->Allocate();
    return std::make_shared<Texture>(TextureType::Texture2D, image);
}

auto GetDefaultSampler()
{
    static std::shared_ptr<Sampler> sampler;
    if (sampler == nullptr) {
        sampler = std::make_shared<Sampler>();
    }
    return sampler;
}

auto& GetWhiteTexture()
{
    static std::shared_ptr<Texture> texture;
    if (texture != nullptr)
        return texture;
    texture = CreateSGTexture(PixelSizedFormat::Uint8_NormalizedRGBA, { 1, 1, 1 });
    ImageFill(*(*texture)[0], { 1, 1, 1, 1 });
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
    static std::shared_ptr<Texture> texture;
    if (texture != nullptr)
        return texture;
    texture = CreateSGTexture(PixelSizedFormat::Uint8_NormalizedRGBA, { 1, 1, 1 });
    ImageFill(*(*texture)[0], { 0.5, 0.5, 1.0, 1.0 });
    return texture;
}

void Material::Set(
    Renderer::Impl& a_Renderer,
    const Msg::Material& a_SGMaterial)
{
    if (a_SGMaterial.HasExtension<MaterialExtensionBase>()) {
        auto& baseExtension = a_SGMaterial.GetExtension<MaterialExtensionBase>();
        _LoadBaseExtension(a_Renderer, baseExtension);
        unlit = baseExtension.unlit;
    } else
        _LoadBaseExtension(a_Renderer, {});
    if (a_SGMaterial.HasExtension<MaterialExtensionSpecularGlossiness>())
        _LoadSpecGlossExtension(a_Renderer, a_SGMaterial.GetExtension<MaterialExtensionSpecularGlossiness>());
    else if (a_SGMaterial.HasExtension<MaterialExtensionMetallicRoughness>())
        _LoadMetRoughExtension(a_Renderer, a_SGMaterial.GetExtension<MaterialExtensionMetallicRoughness>());
    else
        _LoadSpecGlossExtension(a_Renderer, {});
    buffer->Update();
}

void FillTextureInfo(
    GLSL::TextureInfo& a_Info,
    const MaterialTextureInfo& a_SGTextureInfo)
{
    a_Info.texCoord           = a_SGTextureInfo.texCoord;
    a_Info.transform.offset   = a_SGTextureInfo.transform.offset;
    a_Info.transform.rotation = a_SGTextureInfo.transform.rotation;
    a_Info.transform.scale    = a_SGTextureInfo.transform.scale;
}

void Material::_LoadBaseExtension(
    Renderer::Impl& a_Renderer,
    const MaterialExtensionBase& a_Extension)
{
    auto UBOData             = buffer->Get();
    auto& extension          = UBOData.base;
    extension.emissiveFactor = a_Extension.emissiveFactor;
    {
        auto& SGTextureInfo    = a_Extension.occlusionTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultOcclusion() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_BASE_OCCLUSION);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_BASE_OCCLUSION];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.sparseTextureLoader(a_Renderer, SGTexture);
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.emissiveTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultEmissive() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_BASE_EMISSIVE);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_BASE_EMISSIVE];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.sparseTextureLoader(a_Renderer, SGTexture);
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.normalTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultNormal() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_BASE_NORMAL);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_BASE_NORMAL];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.sparseTextureLoader(a_Renderer, SGTexture);
        extension.normalScale  = SGTextureInfo.scale;
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    if (a_Extension.alphaMode == MaterialExtensionBase::AlphaMode::Opaque) {
        extension.alphaCutoff = 0;
        alphaMode             = MATERIAL_ALPHA_OPAQUE;
    } else if (a_Extension.alphaMode == MaterialExtensionBase::AlphaMode::Blend) {
        extension.alphaCutoff = 1;
        alphaMode             = MATERIAL_ALPHA_BLEND;
    } else if (a_Extension.alphaMode == MaterialExtensionBase::AlphaMode::Mask) {
        extension.alphaCutoff = a_Extension.alphaCutoff;
        alphaMode             = MATERIAL_ALPHA_CUTOFF;
    }
    doubleSided = a_Extension.doubleSided;
    buffer->Set(UBOData);
}

void Material::_LoadSpecGlossExtension(
    Renderer::Impl& a_Renderer,
    const MaterialExtensionSpecularGlossiness& a_Extension)
{
    type                       = MATERIAL_TYPE_SPECULAR_GLOSSINESS;
    auto UBOData               = buffer->Get();
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
        textureSampler.texture = a_Renderer.sparseTextureLoader(a_Renderer, SGTexture);
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.specularGlossinessTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultSpecGloss() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_SPECGLOSS_SG);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_SPECGLOSS_SG];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.sparseTextureLoader(a_Renderer, SGTexture);
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    buffer->Set(UBOData);
}

void Material::_LoadMetRoughExtension(
    Renderer::Impl& a_Renderer,
    const MaterialExtensionMetallicRoughness& a_Extension)
{
    type                      = MATERIAL_TYPE_METALLIC_ROUGHNESS;
    auto UBOData              = buffer->Get();
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
        textureSampler.texture = a_Renderer.sparseTextureLoader(a_Renderer, SGTexture);
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    {
        auto& SGTextureInfo    = a_Extension.metallicRoughnessTexture;
        auto& SGTexture        = SGTextureInfo.textureSampler.texture == nullptr ? GetDefaultMetallicRoughness() : SGTextureInfo.textureSampler.texture;
        auto& SGSampler        = SGTextureInfo.textureSampler.sampler == nullptr ? GetDefaultSampler() : SGTextureInfo.textureSampler.sampler;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_METROUGH_MR);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_METROUGH_MR];
        textureSampler.sampler = a_Renderer.LoadSampler(SGSampler.get());
        textureSampler.texture = a_Renderer.sparseTextureLoader(a_Renderer, SGTexture);
        FillTextureInfo(textureInfo, SGTextureInfo);
    }
    buffer->Set(UBOData);
}
}
