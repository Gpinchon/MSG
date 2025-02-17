#include <MSG/Image/Clamp.hpp>
#include <MSG/Image/Cubemap.hpp>
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

int WrapTexelCoord(const MSG::SamplerWrap& a_Wrap, const int& a_TextureSize, const int& a_Coord)
{
    const auto& coord = a_Coord;
    const auto& size  = a_TextureSize;
    switch (a_Wrap) {
    case MSG::SamplerWrap::Repeat:
        return std::fmod(coord, size);
    case MSG::SamplerWrap::ClampToBorder:
        return std::clamp(coord, -1, size);
    case MSG::SamplerWrap::ClampToEdge:
        return std::clamp(coord, 0, size - 1);
    case MSG::SamplerWrap::MirroredRepeat:
        return (size - 1) - Mirror(std::fmod(coord, 2 * size)) - size;
    case MSG::SamplerWrap::MirroredClampToEdge:
        return std::clamp(Mirror(coord), 0, size - 1);
    default:
        break;
    }
    return a_Coord;
}

glm::ivec3 WrapTexelCoords(const MSG::SamplerWrapModes& a_SamplerWrapModes, const glm::uvec3& a_TextureSize, const glm::ivec3& a_TexelCoord)
{
    return {
        WrapTexelCoord(a_SamplerWrapModes[0], a_TextureSize[0], a_TexelCoord[0]),
        WrapTexelCoord(a_SamplerWrapModes[1], a_TextureSize[1], a_TexelCoord[1]),
        WrapTexelCoord(a_SamplerWrapModes[2], a_TextureSize[2], a_TexelCoord[2])
    };
}

bool IsClampedToBorder(const MSG::Image& a_Image, const glm::ivec3& a_TexCoord)
{
    return glm::any(glm::lessThan(a_TexCoord, { 0, 0, 0 }))
        || glm::any(glm::greaterThan(a_TexCoord, glm::ivec3(a_Image.GetSize() - 1u)));
}

glm::vec4 TexelFetchImage(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::ivec3& a_TexCoord)
{
    auto texCoord = WrapTexelCoords(a_Sampler.GetWrapModes(), a_Image.GetSize(), a_TexCoord);
    if (IsClampedToBorder(a_Image, texCoord))
        return a_Sampler.GetBorderColor();
    return a_Image.Load(texCoord);
}

/// Sampler1D functions
glm::vec4 SampleImage1D(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::vec1& a_UV)
{
    auto tcMax = glm::vec3(a_Image.GetSize() - 1u);
    auto tcf   = glm::vec3(a_UV, 0, 0) * tcMax;
    if (GetImageFilter(a_Sampler) == MSG::SamplerFilter::Nearest)
        return TexelFetchImage(a_Sampler, a_Image, MSG::ManhattanRound(tcf));
    auto tx  = glm::fract(tcf.x);
    auto tci = glm::ivec3(glm::round(tcf));
    auto tc0 = tci + 0;
    auto tc1 = tci + 1;
    auto c0  = TexelFetchImage(a_Sampler, a_Image, { tc0.x, tc0.y, tc0.z });
    auto c1  = TexelFetchImage(a_Sampler, a_Image, { tc1.x, tc0.y, tc0.z });
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
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), texCoord);
}

/// Sampler2D functions
glm::vec4 SampleImage2D(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::vec2& a_UV)
{
    auto tcMax = glm::vec3(a_Image.GetSize() - 1u);
    auto tcf   = glm::vec3(a_UV, 0) * tcMax;
    if (GetImageFilter(a_Sampler) == MSG::SamplerFilter::Nearest)
        return TexelFetchImage(a_Sampler, a_Image, MSG::ManhattanRound(tcf));
    auto tx  = glm::fract(tcf.x);
    auto ty  = glm::fract(tcf.y);
    auto tci = glm::ivec3(glm::round(tcf));
    auto tc0 = tci + 0;
    auto tc1 = tci + 1;
    auto c00 = TexelFetchImage(a_Sampler, a_Image, { tc0.x, tc0.y, tc0.z });
    auto c10 = TexelFetchImage(a_Sampler, a_Image, { tc1.x, tc0.y, tc0.z });
    auto c01 = TexelFetchImage(a_Sampler, a_Image, { tc0.x, tc1.y, tc0.z });
    auto c11 = TexelFetchImage(a_Sampler, a_Image, { tc1.x, tc1.y, tc0.z });
    return MSG::PixelBilinearFilter(tx, ty, c00, c10, c01, c11);
}

glm::vec4 MSG::Sampler2D::Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter(*this) == MSG::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(ManhattanRound(a_Lod));
        return SampleImage2D(*this, image, a_UV);
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
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), texCoord);
}

// Sampler3D functions
glm::vec4 SampleImage3D(const MSG::Sampler& a_Sampler, const MSG::Image& a_Image, const glm::vec3& a_UV)
{
    auto tcMax = glm::vec3(a_Image.GetSize() - 1u);
    auto tcf   = a_UV * tcMax;
    if (GetImageFilter(a_Sampler) == MSG::SamplerFilter::Nearest)
        return TexelFetchImage(a_Sampler, a_Image, MSG::ManhattanRound(tcf));
    auto tx   = glm::fract(tcf.x);
    auto ty   = glm::fract(tcf.y);
    auto tz   = glm::fract(tcf.z);
    auto tci  = glm::ivec3(glm::round(tcf));
    auto tc0  = tci + 0;
    auto tc1  = tci + 1;
    auto c000 = TexelFetchImage(a_Sampler, a_Image, { tc0.x, tc0.y, tc0.z });
    auto c100 = TexelFetchImage(a_Sampler, a_Image, { tc1.x, tc0.y, tc0.z });
    auto c010 = TexelFetchImage(a_Sampler, a_Image, { tc0.x, tc1.y, tc0.z });
    auto c110 = TexelFetchImage(a_Sampler, a_Image, { tc1.x, tc1.y, tc0.z });
    auto c001 = TexelFetchImage(a_Sampler, a_Image, { tc0.x, tc0.y, tc1.z });
    auto c101 = TexelFetchImage(a_Sampler, a_Image, { tc1.x, tc0.y, tc1.z });
    auto c011 = TexelFetchImage(a_Sampler, a_Image, { tc0.x, tc1.y, tc1.z });
    auto c111 = TexelFetchImage(a_Sampler, a_Image, { tc1.x, tc1.y, tc1.z });
    auto e    = MSG::PixelBilinearFilter(tx, ty, c000, c100, c010, c110);
    auto f    = MSG::PixelBilinearFilter(tx, ty, c001, c101, c011, c111);
    return glm::mix(e, f, tz);
}

glm::vec4 MSG::Sampler3D::Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter(*this) == MSG::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(ManhattanRound(a_Lod));
        return SampleImage3D(*this, image, a_UV);
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
    auto uvw      = CubemapSampleVecToUVW(a_UV);
    auto wrapMode = GetImageFilter(*this) == SamplerFilter::Nearest ? SamplerWrap::ClampToEdge : SamplerWrap::ClampToBorder;
    Sampler2DArray tempSampler(*this);
    tempSampler.SetWrapModes({ wrapMode, wrapMode, wrapMode });
    return tempSampler.Sample(a_Texture, uvw, a_Lod);
}

// Sampler1DArray functions
glm::vec4 MSG::Sampler1DArray::Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y / (a_Texture.GetSize().y - 1u), 0);
    return Sampler2D { *this }.Sample(a_Texture, uvw, a_Lod);
}
glm::vec4 MSG::Sampler1DArray::TexelFetch(const Texture& a_Texture, const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const
{
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), { a_TexelCoord, 0 });
}

// Sampler2DArray functions
glm::vec4 MSG::Sampler2DArray::Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y, a_UV.z / (a_Texture.GetSize().z - 1u));
    return Sampler3D { *this }.Sample(a_Texture, uvw, a_Lod);
}
glm::vec4 MSG::Sampler2DArray::TexelFetch(const Texture& a_Texture, const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const
{
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), a_TexelCoord);
}
