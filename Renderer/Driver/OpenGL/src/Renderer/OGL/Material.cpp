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
auto CreateSGTexture(const std::shared_ptr<SG::TextureSampler>& a_Sampler, const glm::ivec3& a_Size, const SG::Pixel::Description& a_PixelDesc)
{
    auto texture    = std::make_shared<SG::Texture>(SG::Texture::Type::Texture2D);
    auto bufferView = std::make_shared<SG::BufferView>(0, a_Size.x * a_Size.y * a_Size.z * a_PixelDesc.GetSize());
    auto image      = std::make_shared<SG::Image>(SG::Image::Type::Image2D, a_PixelDesc, a_Size, bufferView);
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
    if (texture == nullptr) {
        glm::ivec3 imageSize = { 4, 4, 1 };
        texture              = CreateSGTexture(GetDefaultSampler(), imageSize, SG::Pixel::SizedFormat::Uint8_NormalizedRGBA);
        auto& image          = texture->GetImage();
        for (auto z = 0u; z < imageSize.z; ++z) {
            for (auto y = 0u; y < imageSize.y; ++y) {
                for (auto x = 0u; x < imageSize.x; ++x) {
                    auto total = x + y + z;
                    auto color = (total % 2 == 0) ? glm::vec4 { 1, 1, 0, 1 } : glm::vec4 { 0, 1, 1, 1 };
                    image->SetColor({ x, y, z }, color);
                }
            }
        }
    }
    return texture;
}

auto GetDefaultNormal()
{
    static std::shared_ptr<SG::Texture> texture;
    if (texture == nullptr) {
        glm::ivec3 imageSize = { 1, 1, 1 };
        texture              = CreateSGTexture(GetDefaultSampler(), imageSize, SG::Pixel::SizedFormat::Uint8_NormalizedRGB);
        auto& image          = texture->GetImage();
        for (auto z = 0u; z < imageSize.z; ++z) {
            for (auto y = 0u; y < imageSize.y; ++y) {
                for (auto x = 0u; x < imageSize.x; ++x) {
                    image->SetColor({ x, y, z }, { 0.5, 0.5, 1.0, 1.0 });
                }
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
        _LoadSpecGlossExtension(a_Renderer,
            a_SGMaterial.GetExtension<SG::SpecularGlossinessExtension>());
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
    type            = MATERIAL_TYPE_SPECULAR_GLOSSINESS;
    auto UBOData    = GetData();
    auto& extension = UBOData.specularGlossiness;
    {
        auto& SGTexture        = a_Extension.diffuseTexture;
        auto& texture          = SGTexture.texture == nullptr ? GetDefaultDiffuse() : SGTexture.texture;
        auto& textureSampler   = textureSamplers.at(SAMPLERS_MATERIAL_SPECGLOSS_DIFF);
        auto& textureInfo      = UBOData.textureInfos[SAMPLERS_MATERIAL_SPECGLOSS_DIFF];
        textureSampler.sampler = a_Renderer.LoadSampler(texture->GetSampler().get());
        textureSampler.texture = a_Renderer.LoadTexture(texture->GetImage().get());
        FillTextureInfo(textureInfo, SGTexture);
    }
    extension.diffuseFactor    = a_Extension.diffuseFactor;
    extension.specularFactor   = a_Extension.specularFactor;
    extension.glossinessFactor = a_Extension.glossinessFactor;
    SetData(UBOData);
}
}
