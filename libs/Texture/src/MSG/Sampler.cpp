#include <MSG/Image/Clamp.hpp>
#include <MSG/Image/ManhattanRound.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>

#include <algorithm>
#include <glm/common.hpp>

MSG::SamplerFilter GetImageFilter(const MSG::Sampler& a_Sampler)
{
    switch (a_Sampler.GetMagFilter()) {
    case MSG::SamplerFilter::Nearest:
    case MSG::SamplerFilter::NearestMipmapLinear:
    case MSG::SamplerFilter::NearestMipmapNearest:
        return MSG::SamplerFilter::Nearest;
    case MSG::SamplerFilter::Linear:
    case MSG::SamplerFilter::LinearMipmapLinear:
    case MSG::SamplerFilter::LinearMipmapNearest:
        return MSG::SamplerFilter::Linear;
    default:
        break;
    }
    return MSG::SamplerFilter::Unknown;
}

MSG::SamplerFilter GetMipmapFilter(const MSG::Sampler& a_Sampler)
{
    switch (a_Sampler.GetMagFilter()) {
    case MSG::SamplerFilter::Nearest:
    case MSG::SamplerFilter::NearestMipmapNearest:
    case MSG::SamplerFilter::LinearMipmapNearest:
        return MSG::SamplerFilter::Nearest;
    case MSG::SamplerFilter::Linear:
    case MSG::SamplerFilter::LinearMipmapLinear:
    case MSG::SamplerFilter::NearestMipmapLinear:
        return MSG::SamplerFilter::Linear;
    default:
        break;
    }
    return MSG::SamplerFilter::Unknown;
}

template <typename T>
T Mirror(const T& a_Val) { return a_Val >= 0 ? a_Val : -(1 + a_Val); }

template <typename T>
T WrapTexelCoord(const MSG::SamplerWrap& a_Wrap, const T& a_TextureSize, const T& a_Coord)
{
    const auto& coord = a_Coord;
    const auto& size  = a_TextureSize;
    switch (a_Wrap) {
    case MSG::SamplerWrap::Repeat:
        return std::fmod(coord, size);
    case MSG::SamplerWrap::ClampToBorder:
        return std::clamp(coord, T(-1), size);
    case MSG::SamplerWrap::ClampToEdge:
        return std::clamp(coord, T(0), size - 1);
    case MSG::SamplerWrap::MirroredRepeat:
        return (size - 1) - Mirror(std::fmod(coord, 2 * size)) - size;
    case MSG::SamplerWrap::MirroredClampToEdge:
        return std::clamp(Mirror(coord), T(0), size - 1);
    default:
        break;
    }
    return a_Coord;
}

template <typename T>
T WrapTexelCoords(const MSG::SamplerWrapModes& a_SamplerWrapModes, const T& a_TextureSize, const T& a_TexelCoord)
{
    return {
        WrapTexelCoord(a_SamplerWrapModes[0], a_TextureSize[0], a_TexelCoord[0]),
        WrapTexelCoord(a_SamplerWrapModes[1], a_TextureSize[1], a_TexelCoord[1]),
        WrapTexelCoord(a_SamplerWrapModes[2], a_TextureSize[2], a_TexelCoord[2])
    };
}

glm::vec4 TexelFetchImage(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::uvec3& a_TexCoord)
{
    return a_Image.Load(WrapTexelCoords(a_Sampler.GetWrapModes(), a_Image.GetSize(), a_TexCoord));
}

/// Sampler1D functions
glm::vec4 SampleImage1D(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::vec1& a_UV)
{
    auto imageSize = glm::vec3(a_Image.GetSize());
    auto tc        = glm::vec3(a_UV, 1, 1) * imageSize;
    auto tc0       = WrapTexelCoords(a_Sampler.GetWrapModes(), imageSize, tc + 0.f);
    if (GetImageFilter(a_Sampler) == MSG::SamplerFilter::Nearest)
        return a_Image.Load(MSG::ManhattanRound(tc0));
    auto tc1 = WrapTexelCoords(a_Sampler.GetWrapModes(), imageSize, tc + 1.f);
    auto tx  = glm::fract(tc.x);
    auto c0  = a_Image.Load({ tc0.x, tc0.y, tc0.z });
    auto c1  = a_Image.Load({ tc1.x, tc0.y, tc0.z });
    return glm::mix(c0, c1, tx);
}

glm::vec4 MSG::Sampler1D::Sample(const Texture& a_Texture, const glm::vec1& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter(*this) == MSG::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(ManhattanRound(a_Lod));
        return SampleImage1D(*this, image, a_UV);
    }
    auto lodFract = glm::fract(a_Lod);
    auto& image0  = *a_Texture.at(glm::floor(a_Lod));
    auto color0   = SampleImage1D(*this, image0, a_UV);
    auto& image1  = *a_Texture.at(glm::ceil(a_Lod));
    auto color1   = SampleImage1D(*this, image1, a_UV);
    return glm::mix(color0, color1, lodFract);
}
glm::vec4 MSG::Sampler1D::TexelFetch(const Texture& a_Texture, const glm::ivec1& a_TexelCoord, const uint32_t& a_Lod) const
{
    glm::ivec3 texCoord(a_TexelCoord.x, 0, 0);
    return Sampler3D { *this }.TexelFetch(a_Texture, texCoord, a_Lod);
}

/// Sampler2D functions
glm::vec4 SampleImage2D(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::vec2& a_UV)
{
    auto imageSize = glm::vec3(a_Image.GetSize());
    auto tc        = glm::vec3(a_UV, 1) * imageSize;
    auto tc0       = WrapTexelCoords(a_Sampler.GetWrapModes(), imageSize, tc + 0.f);
    if (GetImageFilter(a_Sampler) == MSG::SamplerFilter::Nearest)
        return a_Image.Load(MSG::ManhattanRound(tc0));
    auto tc1 = WrapTexelCoords(a_Sampler.GetWrapModes(), imageSize, tc + 1.f);
    auto tx  = glm::fract(tc.x);
    auto ty  = glm::fract(tc.y);
    auto c00 = a_Image.Load({ tc0.x, tc0.y, tc0.z });
    auto c10 = a_Image.Load({ tc1.x, tc0.y, tc0.z });
    auto c01 = a_Image.Load({ tc0.x, tc1.y, tc0.z });
    auto c11 = a_Image.Load({ tc1.x, tc1.y, tc0.z });
    return MSG::PixelBilinearFilter(tx, ty, c00, c10, c01, c11);
}

glm::vec4 MSG::Sampler2D::Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter(*this) == MSG::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(ManhattanRound(a_Lod));
        return SampleImage1D(*this, image, a_UV);
    }
    auto lodFract = glm::fract(a_Lod);
    auto& image0  = *a_Texture.at(glm::floor(a_Lod));
    auto color0   = SampleImage2D(*this, image0, a_UV);
    auto& image1  = *a_Texture.at(glm::ceil(a_Lod));
    auto color1   = SampleImage2D(*this, image1, a_UV);
    return glm::mix(color0, color1, lodFract);
}
glm::vec4 MSG::Sampler2D::TexelFetch(const Texture& a_Texture, const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const
{
    glm::ivec3 texCoord(a_TexelCoord.x, a_TexelCoord.y, 0);
    return Sampler3D { *this }.TexelFetch(a_Texture, texCoord, a_Lod);
}

// Sampler3D functions
glm::vec4 SampleImage3D(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::vec3& a_UV)
{
    auto imageSize = glm::vec3(a_Image.GetSize());
    auto tc        = a_UV * imageSize;
    auto tc0       = WrapTexelCoords(a_Sampler.GetWrapModes(), imageSize, tc + 0.f);
    if (GetImageFilter(a_Sampler) == MSG::SamplerFilter::Nearest)
        return a_Image.Load(MSG::ManhattanRound(tc0));
    auto tc1  = WrapTexelCoords(a_Sampler.GetWrapModes(), imageSize, tc + 1.f);
    auto tx   = glm::fract(tc.x);
    auto ty   = glm::fract(tc.y);
    auto tz   = glm::fract(tc.z);
    auto c000 = a_Image.Load({ tc0.x, tc0.y, tc0.z });
    auto c100 = a_Image.Load({ tc1.x, tc0.y, tc0.z });
    auto c010 = a_Image.Load({ tc0.x, tc1.y, tc0.z });
    auto c110 = a_Image.Load({ tc1.x, tc1.y, tc0.z });
    auto c001 = a_Image.Load({ tc0.x, tc0.y, tc1.z });
    auto c101 = a_Image.Load({ tc1.x, tc0.y, tc1.z });
    auto c011 = a_Image.Load({ tc0.x, tc1.y, tc1.z });
    auto c111 = a_Image.Load({ tc1.x, tc1.y, tc1.z });
    auto e    = MSG::PixelBilinearFilter(tx, ty, c000, c100, c010, c110);
    auto f    = MSG::PixelBilinearFilter(tx, ty, c001, c101, c011, c111);
    return glm::mix(e, f, tz);
}

glm::vec4 MSG::Sampler3D::Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter(*this) == MSG::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(ManhattanRound(a_Lod));
        return SampleImage1D(*this, image, a_UV);
    }
    auto lodFract = glm::fract(a_Lod);
    auto& image0  = *a_Texture.at(glm::floor(a_Lod));
    auto color0   = SampleImage3D(*this, image0, a_UV);
    auto& image1  = *a_Texture.at(glm::ceil(a_Lod));
    auto color1   = SampleImage3D(*this, image1, a_UV);
    return glm::mix(color0, color1, lodFract);
}
glm::vec4 MSG::Sampler3D::TexelFetch(const Texture& a_Texture, const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const
{
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), a_TexelCoord);
}

// SamplerCube functions
glm::vec4 MSG::SamplerCube::Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod) const
{
    auto& v        = a_UV;
    glm::vec3 vAbs = abs(v);
    float ma;
    glm::vec2 uv;
    float faceIndex;
    if (vAbs.z >= vAbs.x && vAbs.z >= vAbs.y) {
        faceIndex = v.z < 0.f ? 5.f : 4.f;
        ma        = 0.5f / vAbs.z;
        uv        = glm::vec2(v.z < 0.0 ? -v.x : v.x, -v.y);
    } else if (vAbs.y >= vAbs.x) {
        faceIndex = v.y < 0.f ? 3.f : 2.f;
        ma        = 0.5f / vAbs.y;
        uv        = glm::vec2(v.x, v.y < 0.f ? -v.z : v.z);
    } else {
        faceIndex = v.x < 0.f ? 1.f : 0.f;
        ma        = 0.5f / vAbs.x;
        uv        = glm::vec2(v.x < 0.f ? v.z : -v.z, -v.y);
    }
    glm::vec3 uvw { uv * ma + 0.5f, faceIndex };
    return Sampler2DArray { *this }.Sample(a_Texture, uvw, a_Lod);
}

// Sampler1DArray functions
glm::vec4 MSG::Sampler1DArray::Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y / a_Texture.GetSize().y, 0);
    return Sampler2D { *this }.Sample(a_Texture, uvw, a_Lod);
}
glm::vec4 MSG::Sampler1DArray::TexelFetch(const Texture& a_Texture, const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const
{
    glm::ivec3 texCoord(a_TexelCoord, 0);
    return Sampler2D { *this }.TexelFetch(a_Texture, texCoord, a_Lod);
}

// Sampler2DArray functions
glm::vec4 MSG::Sampler2DArray::Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y, a_UV.z / a_Texture.GetSize().z);
    return Sampler3D { *this }.Sample(a_Texture, uvw, a_Lod);
}
glm::vec4 MSG::Sampler2DArray::TexelFetch(const Texture& a_Texture, const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const
{
    return Sampler3D { *this }.TexelFetch(a_Texture, a_TexelCoord, a_Lod);
}
