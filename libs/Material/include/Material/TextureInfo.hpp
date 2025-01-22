#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Texture/Sampler.hpp>
#include <memory>

#include <glm/vec2.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
struct MaterialTextureInfo {
    TextureSampler textureSampler;
    uint32_t texCoord { 0 };
    struct Transform {
        glm::vec2 offset { 0, 0 };
        glm::vec2 scale { 1, 1 };
        float rotation { 0 };
    } transform;
};
struct NormalTextureInfo : MaterialTextureInfo {
    NormalTextureInfo() = default;
    NormalTextureInfo(const MaterialTextureInfo& a_Info)
        : MaterialTextureInfo(a_Info)
    {
    }
    float scale { 1 };
};
struct OcclusionTextureInfo : MaterialTextureInfo {
    OcclusionTextureInfo() = default;
    OcclusionTextureInfo(const MaterialTextureInfo& a_Info)
        : MaterialTextureInfo(a_Info)
    {
    }
    float strength { 1 };
};
}
