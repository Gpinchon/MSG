/*
* @Author: gpinchon
* @Date:   2021-01-04 09:42:56
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-02-22 18:30:45
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>
#include <Shader/Stage.hpp>

#include <glm/glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <set>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Material::Extensions {
class Extension : public Core::Inherit<Core::Object, Extension> {
public:
    /** @return the shader's Fragment stage extension code */
    inline void SetCode(const Shader::Stage::Code& code) {
        _code = code;
    }
    inline void SetColor(const std::string& name, const glm::vec3& value) {
        _colors[name] = value;
    }
    inline void SetValue(const std::string& name, const float value) {
        _values[name] = value;
    }
    inline void SetDefine(const std::string& name, const std::string& value = "") {
        _defines[name] = value;
    }
    inline void RemoveDefine(const std::string& name) {
        _defines.erase(name);
    }
    inline void SetTexture(const std::string& name, const std::shared_ptr<Textures::Texture>& value) {
        _textures[name] = value;
    }
    inline auto& GetCode() const {
        return _code;
    }
    inline auto GetColor(const std::string& name) const {
        auto value{ _colors.find(name) };
        return value == _colors.end() ? glm::vec3(0) : value->second;
    }
    inline auto GetValue(const std::string& name) const {
        auto& value{ _values.find(name) };
        return value == _values.end() ? float(0) : value->second;
    }
    inline auto GetDefine(const std::string& name) const {
        auto value{ _defines.find(name) };
        return value == _defines.end() ? "" : value->second;
    }
    inline auto GetTexture(const std::string& name) const {
        auto value{ _textures.find(name) };
        return value == _textures.end() ? nullptr : value->second;
    }

    /** @return the extension's colors map */
    inline auto& GetColors() const {
        return _colors;
    }
    /** @return the extension's values map */
    inline auto& GetValues() const {
        return _values;
    }
    /** @return the extension's defines map */
    inline auto& GetDefines() const {
        return _defines;
    }
    /** @return the extension's textures map */
    inline auto& GetTextures() const {
        return _textures;
    }

protected:
    Extension(const std::string& name) : Inherit(name) {}
private:
    Shader::Stage::Code _code;
    std::map<std::string, glm::vec3> _colors;
    std::map<std::string, float> _values;
    std::map<std::string, std::string> _defines;
    std::map<std::string, std::shared_ptr<Textures::Texture>> _textures;
};
}