#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/Material/Extension.hpp>
#include <MSG/Material/TextureInfo.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
struct MaterialExtensionBase : MaterialExtension {
    enum class AlphaMode {
        Opaque,
        Mask,
        Blend,
        MaxValue
    };
    NormalTextureInfo normalTexture       = {};
    OcclusionTextureInfo occlusionTexture = {};
    MaterialTextureInfo emissiveTexture   = {};
    glm::vec3 emissiveFactor              = { 0, 0, 0 };
    AlphaMode alphaMode                   = { AlphaMode::Opaque };
    float alphaCutoff                     = { 0.5 };
    bool doubleSided                      = { false };
    bool unlit                            = { false };
};
}
