#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>

#include <glm/fwd.hpp>
#include <glm/vec4.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Texture;
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
enum class SamplerWrap {
    Unknown = -1,
    Repeat,
    ClampToBorder,
    ClampToEdge,
    MirroredRepeat,
    MirroredClampToEdge,
    MaxValue
};
enum class SamplerFilter {
    Unknown = -1,
    Nearest,
    Linear,
    NearestMipmapLinear,
    NearestMipmapNearest,
    LinearMipmapLinear,
    LinearMipmapNearest,
    MaxValue
};
enum class SamplerCompareMode {
    Unknown = -1,
    None,
    CompareRefToTexture,
    MaxValue
};
enum class SamplerCompareFunc {
    Unknown = -1,
    LessEqual,
    GreaterEqual,
    Less,
    Greater,
    Equal,
    NotEqual,
    Always,
    Never,
    MaxValue
};
using SamplerWrapModes = std::array<SamplerWrap, 3>;

/** @brief Texture Sampler defines how the texture is sampled in shader */
class Sampler : public Core::Inherit<Core::Object, Sampler> {
public:
    PROPERTY(SamplerFilter, MagFilter, SamplerFilter::Linear);
    PROPERTY(SamplerFilter, MinFilter, SamplerFilter::NearestMipmapLinear);
    PROPERTY(float, MinLOD, -1000);
    PROPERTY(float, MaxLOD, 1000);
    PROPERTY(float, LODBias, 0);
    PROPERTY(float, MaxAnisotropy, 16);
    PROPERTY(SamplerWrapModes, WrapModes, { SamplerWrap::Repeat, SamplerWrap::Repeat, SamplerWrap::Repeat });
    PROPERTY(SamplerCompareMode, CompareMode, SamplerCompareMode::None);
    PROPERTY(SamplerCompareFunc, CompareFunc, SamplerCompareFunc::Always);
    PROPERTY(glm::vec4, BorderColor, 0.f, 0.f, 0.f, 1.f);

public:
    MSG::SamplerFilter GetImageFilter() const;
    MSG::SamplerFilter GetMipmapFilter() const;
    auto& GetWrapS() const { return GetWrapModes().at(0); }
    auto& GetWrapT() const { return GetWrapModes().at(1); }
    auto& GetWrapR() const { return GetWrapModes().at(2); }
    void SetWrapS(const SamplerWrap& a_Mode) { GetWrapModes().at(0) = a_Mode; }
    void SetWrapT(const SamplerWrap& a_Mode) { GetWrapModes().at(1) = a_Mode; }
    void SetWrapR(const SamplerWrap& a_Mode) { GetWrapModes().at(2) = a_Mode; }
};

class Sampler1D : public Sampler {
public:
    using Sampler::Sampler;
    Sampler1D(const Sampler& a_Other)
        : Sampler(a_Other)
    {
    }
    glm::vec4 Sample(const Image& a_Image, const glm::vec1& a_UV) const;
    glm::vec4 Sample(const Texture& a_Texture, const glm::vec1& a_UV, const float& a_Lod = 0) const;
    glm::vec4 TexelFetch(const Texture& a_Texture, const glm::ivec1& a_TexelCoord, const uint32_t& a_Lod) const;
};

class Sampler2D : public Sampler {
public:
    using Sampler::Sampler;
    Sampler2D(const Sampler& a_Other)
        : Sampler(a_Other)
    {
    }
    glm::vec4 Sample(const Image& a_Image, const glm::vec2& a_UV) const;
    glm::vec4 Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod = 0) const;
    glm::vec4 TexelFetch(const Texture& a_Texture, const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const;
};

class Sampler3D : public Sampler {
public:
    using Sampler::Sampler;
    Sampler3D(const Sampler& a_Other)
        : Sampler(a_Other)
    {
    }
    glm::vec4 Sample(const Image& a_Image, const glm::vec3& a_UV) const;
    glm::vec4 Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod = 0) const;
    glm::vec4 TexelFetch(const Texture& a_Texture, const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const;
};

class SamplerCube : public Sampler {
public:
    using Sampler::Sampler;
    SamplerCube(const Sampler& a_Other)
        : Sampler(a_Other)
    {
    }
    glm::vec4 Sample(const Image& a_Image, const glm::vec3& a_Dir) const;
    glm::vec4 Sample(const Texture& a_Texture, const glm::vec3& a_Dir, const float& a_Lod = 0) const;
};

class Sampler1DArray : public Sampler {
public:
    using Sampler::Sampler;
    Sampler1DArray(const Sampler& a_Other)
        : Sampler(a_Other)
    {
    }
    glm::vec4 Sample(const Image& a_Image, const glm::vec2& a_UV) const;
    glm::vec4 Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod = 0) const;
    glm::vec4 TexelFetch(const Texture& a_Texture, const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const;
};

class Sampler2DArray : public Sampler {
public:
    using Sampler::Sampler;
    Sampler2DArray(const Sampler& a_Other)
        : Sampler(a_Other)
    {
    }
    glm::vec4 Sample(const Image& a_Image, const glm::vec3& a_UV) const;
    glm::vec4 Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod = 0) const;
    glm::vec4 TexelFetch(const Texture& a_Texture, const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const;
};
}
