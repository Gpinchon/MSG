#include <Material/MetallicRoughness.hpp>
#include <Texture/Texture2D.hpp>

namespace TabGraph::Material::Extensions {
MetallicRoughness::MetallicRoughness() : Inherit("MetallicRoughness")
{
    auto extensionCode =
#include "metallicRoughness.frag"
    ;
    SetCode({ extensionCode, "MetallicRoughness();" });
}

void MetallicRoughness::SetTextureBaseColor(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.BaseColor", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_BASECOLOR");
}

void MetallicRoughness::SetTextureMetallicRoughness(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.MetallicRoughness", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLICROUGHNESS");
}

void MetallicRoughness::SetTextureRoughness(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.Roughness", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_ROUGHNESS");
}

void MetallicRoughness::SetTextureMetallic(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("MetallicRoughnessTextures.Metallic", texture);
    texture ? SetDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC") : RemoveDefine("METALLIC_ROUGHNESS_TEXTURE_USE_METALLIC");
}

}