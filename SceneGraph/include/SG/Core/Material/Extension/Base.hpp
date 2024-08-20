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
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Material/Extension/Extension.hpp>
#include <SG/Core/Material/TextureInfo.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
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
