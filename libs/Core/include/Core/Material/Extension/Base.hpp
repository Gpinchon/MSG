#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Material/Extension/Extension.hpp>
#include <Core/Material/TextureInfo.hpp>
#include <Core/Object.hpp>
#include <Core/Property.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Core {
struct BaseExtension : MaterialExtension {
    enum class AlphaMode {
        Opaque,
        Mask,
        Blend,
        MaxValue
    };
    NormalTextureInfo normalTexture       = {};
    OcclusionTextureInfo occlusionTexture = {};
    TextureInfo emissiveTexture           = {};
    glm::vec3 emissiveFactor              = { 0, 0, 0 };
    AlphaMode alphaMode                   = { AlphaMode::Opaque };
    float alphaCutoff                     = { 0.5 };
    bool doubleSided                      = { false };
    bool unlit                            = { false };
};
}
