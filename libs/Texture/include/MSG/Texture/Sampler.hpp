#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/PixelDescriptor.hpp>

#include <glm/fwd.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Texture;
class Sampler;
}

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
struct TextureSampler {
    std::shared_ptr<Texture> texture;
    std::shared_ptr<Sampler> sampler;
};

struct TextureSampler1D : TextureSampler {
    PixelColor Sample(const glm::vec1& a_UV, const float& a_Lod = 0) const;
    PixelColor TexelFetch(const glm::ivec1& a_TexelCoord, const uint32_t& a_Lod) const;
};

struct TextureSampler2D : TextureSampler {
    PixelColor Sample(const glm::vec2& a_UV, const float& a_Lod = 0) const;
    PixelColor TexelFetch(const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const;
};

struct TextureSampler3D : TextureSampler {
    PixelColor Sample(const glm::vec3& a_UV, const float& a_Lod = 0) const;
    PixelColor TexelFetch(const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const;
};

struct TextureSamplerCube : TextureSampler {
    PixelColor Sample(const glm::vec3& a_UV, const float& a_Lod = 0) const;
};

struct TextureSampler1DArray : TextureSampler {
    PixelColor Sample(const glm::vec2& a_UV, const float& a_Lod = 0) const;
    PixelColor TexelFetch(const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const;
};

struct TextureSampler2DArray : TextureSampler {
    PixelColor Sample(const glm::vec3& a_UV, const float& a_Lod = 0) const;
    PixelColor TexelFetch(const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const;
};
}
