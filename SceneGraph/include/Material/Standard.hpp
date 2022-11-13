/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-03 20:05:02
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Property.hpp>
#include <Material/Extension.hpp>
#include <Renderer/Renderer.hpp>

#include <array>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph {
namespace Shader {
class Program;
}
namespace Textures {
class Texture2D;
}
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Material {
enum class OpacityMode {
    Opaque,
    Mask,
    Blend,
    MaxValue
};
class Standard : public Core::Inherit<Extensions::Extension, Standard> {
public:
    PROPERTY(bool, DoubleSided, false);
    PROPERTY(OpacityMode, OpacityMode, OpacityMode::Opaque);
    PROPERTY(glm::vec2, UVScale, 1);
    PROPERTY(glm::vec2, UVOffset, 0);
    PROPERTY(float, UVRotation, 0);

public:
    Standard(const std::string& name);
    virtual void AddExtension(std::shared_ptr<Extension> extension) final;
    virtual void RemoveExtension(std::shared_ptr<Extension> extension) final;
    virtual std::shared_ptr<Extension> GetExtension(const std::string& name) const final;
    virtual void Bind(const Renderer::Options::Pass& pass);
    auto GetShader(const Renderer::Options::Pass& pass) {
        return _shaders.at(size_t(pass));
    }
    void SetShader(const Renderer::Options::Pass& pass, std::shared_ptr<Shader::Program> shader) {
        _shaders.at(size_t(pass)) = shader;
    }

    inline auto GetDiffuse(void) const {
        return GetColor("StandardDiffuse");
    }
    inline auto GetEmissive(void) {
        return GetColor("StandardEmissive");
    }
    inline auto GetOpacityCutoff(void) {
        return GetValue("StandardOpacityCutoff");
    }
    inline auto GetOpacity(void) {
        return GetValue("StandardOpacity");
    }
    inline auto GetParallax(void) {
        return GetValue("StandardParallax");
    }
    inline auto GetIor(void) {
        return GetValue("StandardIor");
    }
    inline auto GetTextureDiffuse(void) const {
        return GetTexture("StandardTextureDiffuse");
    }
    inline auto GetTextureEmissive(void) const {
        return GetTexture("StandardTextureEmissive");
    }
    inline auto GetTextureNormal(void) const {
        return GetTexture("StandardTextureNormal");
    }
    inline auto GetTextureHeight(void) const {
        return GetTexture("StandardTextureHeight");
    }
    inline auto GetTextureAO(void) const {
        return GetTexture("StandardTextureAO");
    }
    inline auto GetTextureBRDFLUT(void) const {
        return GetTexture("StandardTextureBRDFLUT");
    }

    void SetDiffuse(glm::vec3 value) {
        SetColor("StandardDiffuse", value);
    }
    void SetEmissive(glm::vec3 value) {
        SetColor("StandardEmissive", value);
    }
    void SetOpacityCutoff(float value) {
        SetValue("StandardOpacityCutoff", value);
    }
    void SetOpacity(float value) {
        SetValue("StandardOpacity", value);
    }
    void SetParallax(float value) {
        SetValue("StandardParallax", value);
    }
    void SetIor(float value) {
        SetValue("StandardIor", value);
    }
    void SetTextureDiffuse(std::shared_ptr<Textures::Texture2D>);
    void SetTextureEmissive(std::shared_ptr<Textures::Texture2D>);
    void SetTextureNormal(std::shared_ptr<Textures::Texture2D>);
    void SetTextureHeight(std::shared_ptr<Textures::Texture2D>);
    void SetTextureAO(std::shared_ptr<Textures::Texture2D>);
    void SetTextureBRDFLUT(std::shared_ptr<Textures::Texture2D>);

protected:
    std::array<std::shared_ptr<Shader::Program>, (int)Renderer::Options::Pass::MaxValue> _shaders;
    std::set<std::shared_ptr<Extension>> _extensions;
};
}
