/*
* @Author: gpinchon
* @Date:   2020-10-01 15:30:47
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-18 22:49:43
*/
#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Material/Extension.hpp>
#include <Core/Inherit.hpp>

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
class MetallicRoughness : public Core::Inherit<Extension, MetallicRoughness> {
public:
    MetallicRoughness();
    auto GetTextureBaseColor() const
    {
        return GetTexture("MetallicRoughnessTextures.BaseColor");
    }
    auto GetTextureMetallicRoughness() const
    {
        return GetTexture("MetallicRoughnessTextures.MetallicRoughness");
    }
    auto GetTextureRoughness() const
    {
        return GetTexture("MetallicRoughnessTextures.Roughness");
    }
    auto GetTextureMetallic() const
    {
        return GetTexture("MetallicRoughnessTextures.Metallic");
    }
    auto GetRoughness() const
    {
        return GetValue("MetallicRoughnessRoughness");
    }
    auto GetMetallic() const
    {
        return GetValue("MetallicRoughnessMetallic");
    }
    auto GetOpacity() const
    {
        return GetValue("MetallicRoughnessOpacity");
    }
    auto GetBaseColor() const
    {
        return GetColor("MetallicRoughnessBaseColor");
    }
    void SetTextureBaseColor(std::shared_ptr<Textures::Texture2D> texture);
    void SetTextureMetallicRoughness(std::shared_ptr<Textures::Texture2D> texture);
    void SetTextureRoughness(std::shared_ptr<Textures::Texture2D> texture);
    void SetTextureMetallic(std::shared_ptr<Textures::Texture2D> texture);
    void SetRoughness(float value)
    {
        SetValue("MetallicRoughnessRoughness", value);
    }
    void SetMetallic(float value)
    {
        SetValue("MetallicRoughnessMetallic", value);
    }
    void SetOpacity(float value)
    {
        SetValue("MetallicRoughnessOpacity", value);
    }
    void SetBaseColor(glm::vec3 value)
    {
        SetColor("MetallicRoughnessBaseColor", value);
    }
};
}

