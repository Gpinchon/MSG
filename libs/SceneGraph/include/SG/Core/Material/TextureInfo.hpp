#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Texture/TextureSampler.hpp>
#include <memory>

#include <glm/vec2.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
struct TextureInfo {
    SG::TextureSampler textureSampler;
    uint32_t texCoord { 0 };
    struct Transform {
        glm::vec2 offset { 0, 0 };
        glm::vec2 scale { 1, 1 };
        float rotation { 0 };
    } transform;
};
struct NormalTextureInfo : TextureInfo {
    NormalTextureInfo() = default;
    NormalTextureInfo(const TextureInfo& a_Info)
        : TextureInfo(a_Info)
    {
    }
    float scale { 1 };
};
struct OcclusionTextureInfo : TextureInfo {
    OcclusionTextureInfo() = default;
    OcclusionTextureInfo(const TextureInfo& a_Info)
        : TextureInfo(a_Info)
    {
    }
    float strength { 1 };
};
}
