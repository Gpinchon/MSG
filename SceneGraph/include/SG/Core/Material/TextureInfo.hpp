#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <memory>

#include <glm/vec2.hpp>

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
struct TextureInfo {
    std::shared_ptr<SG::Texture> texture { nullptr };
    uint32_t texCoord { 0 };
    struct {
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
