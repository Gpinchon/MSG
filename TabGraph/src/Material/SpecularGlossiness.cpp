/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:48
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 23:05:42
*/
#include <Material/SpecularGlossiness.hpp>
#include <Texture/Texture2D.hpp>

namespace TabGraph::Material::Extensions {
SpecularGlossiness::SpecularGlossiness()
    : Inherit("SpecularGlossiness")
{
    auto extensionCode =
#include "specularGlossiness.frag"
        ;
    SetCode({ extensionCode, "SpecularGlossiness();" });
}

void SpecularGlossiness::SetTextureDiffuse(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.Diffuse", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_DIFFUSE");
}

void SpecularGlossiness::SetTextureSpecular(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.Specular", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULAR");
}

void SpecularGlossiness::SetTextureGlossiness(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.Glossiness", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_GLOSSINESS");
}

void SpecularGlossiness::SetTextureSpecularGlossiness(std::shared_ptr<Textures::Texture2D> texture)
{
    SetTexture("SpecularGlossinessTextures.SpecularGlossiness", texture);
    texture ? SetDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS") : RemoveDefine("SPECULAR_GLOSSINESS_TEXTURE_USE_SPECULARGLOSSINESS");
}

}