#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Material/TextureInfo.hpp>

#include <glm/vec3.hpp>

#include <map>
#include <string>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Texture;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Material : public Inherit<Object, Material> {
public:
    struct NormalTextureInfo : TextureInfo {
        float scale{ 1 };
    };
    struct OcclusionTextureInfo : TextureInfo {
        float strength{ 1 };
    };
    enum class AlphaMode {
        Opaque,
        Mask,
        Blend,
        MaxValue
    };
    PROPERTY(NormalTextureInfo,     NormalTexture, );
    PROPERTY(OcclusionTextureInfo,  OcclusionTexture, );
    PROPERTY(TextureInfo,           EmissiveTexture, );
    PROPERTY(glm::vec3,             EmissiveFactor, 0, 0, 0);
    PROPERTY(AlphaMode,             AlphaMode, AlphaMode::Opaque);
    PROPERTY(float,                 AlphaCutoff, 0.5);
    PROPERTY(bool,                  DoubleSided, false);
    PROPERTY(bool,                  Unlit, false);

public:
    Material(const std::string& a_Name) : Inherit(a_Name) {}
    template<typename Extension>
    void AddExtension(const Extension& a_Extension) {
        _parameters[a_Extension.GetName()] = std::make_unique<Extension>(a_Extension);
    }
    template<typename Extension>
    auto& GetExtension(const std::string& a_Name) const {
        auto& parameter = _parameters.at(a_Name);
        assert(parameter->IsCompatible(typeid(Extension)));
        return parameter;
    }
    auto& GetExtensions() const { return _parameters; }

private:
    std::map<std::string, std::unique_ptr<Object>> _parameters;
};
}