/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-22 20:46:27
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Material/Extension.hpp>
#include <Core/Inherit.hpp>

#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
    class Texture2D;
}

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Material::Extensions {
class SpecularGlossiness : public Core::Inherit<Extension, SpecularGlossiness> {
public:
    SpecularGlossiness();
    inline auto GetTextureDiffuse() const
    {
        return GetTexture("SpecularGlossinessTextures.Diffuse");
    }
    inline auto GetTextureSpecular() const
    {
        return GetTexture("SpecularGlossinessTextures.Specular");
    }
    inline auto GetTextureGlossiness() const
    {
        return GetTexture("SpecularGlossinessTextures.Glossiness");
    }
    inline auto GetTextureSpecularGlossiness() const
    {
        return GetTexture("SpecularGlossinessTextures.SpecularGlossiness");
    }
    inline auto GetDiffuse() const
    {
        return GetColor("SpecularGlossinessDiffuse");
    }
    inline auto GetSpecular() const
    {
        return GetColor("SpecularGlossinessSpecular");
    }
    inline auto GetGlossines() const
    {
        return GetValue("SpecularGlossinessGlossiness");
    }
    inline auto GetOpacity() const
    {
        return GetValue("SpecularGlossinessOpacity");
    }
    void SetTextureDiffuse(std::shared_ptr<Textures::Texture2D> texture);
    void SetTextureSpecular(std::shared_ptr<Textures::Texture2D> texture);
    void SetTextureGlossiness(std::shared_ptr<Textures::Texture2D> texture);
    void SetTextureSpecularGlossiness(std::shared_ptr<Textures::Texture2D> texture);
    void SetDiffuse(const glm::vec3& value)
    {
        SetColor("SpecularGlossinessDiffuse", value);
    }
    void SetSpecular(const glm::vec3& value)
    {
        SetColor("SpecularGlossinessSpecular", value);
    }
    void SetGlossiness(float value)
    {
        SetValue("SpecularGlossinessGlossiness", value);
    }
    void SetOpacity(float value)
    {
        SetValue("SpecularGlossinessOpacity", value);
    }
};
}