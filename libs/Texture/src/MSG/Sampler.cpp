#include <MSG/Debug.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Texture/ManhattanRound.hpp>

#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/mat3x3.hpp>

#include <algorithm>
#include <numbers>

Msg::SamplerFilter Msg::Sampler::GetImageFilter() const
{
    switch (GetMinFilter()) {
    case Msg::SamplerFilter::Nearest:
    case Msg::SamplerFilter::NearestMipmapLinear:
    case Msg::SamplerFilter::NearestMipmapNearest:
        return Msg::SamplerFilter::Nearest;
    case Msg::SamplerFilter::Linear:
    case Msg::SamplerFilter::LinearMipmapLinear:
    case Msg::SamplerFilter::LinearMipmapNearest:
        return Msg::SamplerFilter::Linear;
    default:
        break;
    }
    return Msg::SamplerFilter::Unknown;
}

Msg::SamplerFilter Msg::Sampler::GetMipmapFilter() const
{
    switch (GetMinFilter()) {
    case Msg::SamplerFilter::Nearest:
    case Msg::SamplerFilter::NearestMipmapNearest:
    case Msg::SamplerFilter::LinearMipmapNearest:
        return Msg::SamplerFilter::Nearest;
    case Msg::SamplerFilter::Linear:
    case Msg::SamplerFilter::LinearMipmapLinear:
    case Msg::SamplerFilter::NearestMipmapLinear:
        return Msg::SamplerFilter::Linear;
    default:
        break;
    }
    return Msg::SamplerFilter::Unknown;
}

template <typename T>
T Mirror(const T& a_Val) { return a_Val >= 0 ? a_Val : -(1 + a_Val); }

int WrapTexelCoord(const Msg::SamplerWrap& a_Wrap, const int& a_TextureSize, const int& a_Coord)
{
    const auto& coord = a_Coord;
    const auto& size  = a_TextureSize;
    switch (a_Wrap) {
    case Msg::SamplerWrap::Repeat:
        return std::fmod(std::fmod(coord, size) + size, size); // handle negative indice as well
    case Msg::SamplerWrap::ClampToBorder:
        return std::clamp(coord, -1, size);
    case Msg::SamplerWrap::ClampToEdge:
        return std::clamp(coord, 0, size - 1);
    case Msg::SamplerWrap::MirroredRepeat:
        return (size - 1) - Mirror(std::fmod(coord, 2 * size)) - size;
    case Msg::SamplerWrap::MirroredClampToEdge:
        return std::clamp(Mirror(coord), 0, size - 1);
    default:
        break;
    }
    return a_Coord;
}

glm::ivec3 WrapTexelCoords(const Msg::SamplerWrapModes& a_SamplerWrapModes, const glm::uvec3& a_TextureSize, const glm::ivec3& a_TexelCoord)
{
    return {
        WrapTexelCoord(a_SamplerWrapModes[0], a_TextureSize[0], a_TexelCoord[0]),
        WrapTexelCoord(a_SamplerWrapModes[1], a_TextureSize[1], a_TexelCoord[1]),
        WrapTexelCoord(a_SamplerWrapModes[2], a_TextureSize[2], a_TexelCoord[2])
    };
}

bool IsClampedToBorder(const Msg::Image& a_Image, const glm::ivec3& a_TexCoord)
{
    return glm::any(glm::equal(a_TexCoord, { -1, -1, -1 }))
        || glm::any(glm::equal(a_TexCoord, glm::ivec3(a_Image.GetSize())));
}

glm::vec4 TexelFetchImage(const Msg::Sampler& a_Sampler, const Msg::Image& a_Image, const glm::ivec3& a_TexCoord)
{
    auto texCoord = WrapTexelCoords(a_Sampler.GetWrapModes(), a_Image.GetSize(), a_TexCoord);
    return IsClampedToBorder(a_Image, texCoord) ? a_Sampler.GetBorderColor() : a_Image.Load(texCoord);
}

/// Sampler1D functions
glm::vec4 Msg::Sampler1D::Sample(const Image& a_Image, const glm::vec1& a_UV) const
{
    auto tcMax = glm::vec3(a_Image.GetSize() - 1u);
    auto tcf   = glm::vec3(a_UV, 0, 0) * tcMax - 0.5f;
    if (GetImageFilter() == Msg::SamplerFilter::Nearest)
        return TexelFetchImage(*this, a_Image, Msg::ManhattanRound(tcf));
    auto tx  = glm::fract(tcf.x);
    auto tc0 = tcf + 0.f;
    auto tc1 = tcf + 1.f;
    auto c0  = TexelFetchImage(*this, a_Image, { tc0.x, tc0.y, tc0.z });
    auto c1  = TexelFetchImage(*this, a_Image, { tc1.x, tc0.y, tc0.z });
    return glm::mix(c0, c1, tx);
}
glm::vec4 Msg::Sampler1D::Sample(const Texture& a_Texture, const glm::vec1& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter() == Msg::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(a_Lod);
        return Sample(image, a_UV);
    }
    auto lodFract = glm::fract(a_Lod);
    auto& image0  = *a_Texture.at(glm::floor(a_Lod));
    auto color0   = Sample(image0, a_UV);
    auto& image1  = *a_Texture.at(glm::ceil(a_Lod));
    auto color1   = Sample(image1, a_UV);
    return glm::mix(color0, color1, lodFract);
}
glm::vec4 Msg::Sampler1D::TexelFetch(const Texture& a_Texture, const glm::ivec1& a_TexelCoord, const uint32_t& a_Lod) const
{
    glm::ivec3 texCoord(a_TexelCoord.x, 0, 0);
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), texCoord);
}

/// Sampler2D functions
glm::vec4 Msg::Sampler2D::Sample(const Image& a_Image, const glm::vec2& a_UV) const
{
    auto tcMax = glm::vec3(a_Image.GetSize() - 1u);
    auto tcf   = glm::vec3(a_UV, 0) * tcMax;
    if (GetImageFilter() == Msg::SamplerFilter::Nearest)
        return TexelFetchImage(*this, a_Image, Msg::ManhattanRound(tcf));
    tcf -= 0.5f;
    auto tcfr = glm::fract(tcf);
    auto tc0  = tcf + 0.f;
    auto tc1  = tcf + 1.f;
    auto c00  = TexelFetchImage(*this, a_Image, { tc0.x, tc0.y, tc0.z });
    auto c10  = TexelFetchImage(*this, a_Image, { tc1.x, tc0.y, tc0.z });
    auto c01  = TexelFetchImage(*this, a_Image, { tc0.x, tc1.y, tc0.z });
    auto c11  = TexelFetchImage(*this, a_Image, { tc1.x, tc1.y, tc0.z });
    return Msg::PixelBilinearFilter(tcfr.x, tcfr.y, c00, c10, c01, c11);
}
glm::vec4 Msg::Sampler2D::Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter() == Msg::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(a_Lod);
        return Sample(image, a_UV);
    }
    auto lodFract = glm::fract(a_Lod);
    auto& image0  = *a_Texture.at(glm::floor(a_Lod));
    auto color0   = Sample(image0, a_UV);
    auto& image1  = *a_Texture.at(glm::ceil(a_Lod));
    auto color1   = Sample(image1, a_UV);
    return glm::mix(color0, color1, lodFract);
}
glm::vec4 Msg::Sampler2D::TexelFetch(const Texture& a_Texture, const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const
{
    glm::ivec3 texCoord(a_TexelCoord.x, a_TexelCoord.y, 0);
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), texCoord);
}

// Sampler3D functions
glm::vec4 Msg::Sampler3D::Sample(const Image& a_Image, const glm::vec3& a_UV) const
{
    auto tcMax = glm::vec3(a_Image.GetSize() - 1u);
    auto tcf   = glm::vec3(a_UV) * tcMax;
    if (GetImageFilter() == Msg::SamplerFilter::Nearest)
        return TexelFetchImage(*this, a_Image, Msg::ManhattanRound(tcf));
    tcf -= 0.5f;
    auto tcfr = glm::fract(tcf);
    auto tc0  = tcf + 0.f;
    auto tc1  = tcf + 1.f;
    auto c000 = TexelFetchImage(*this, a_Image, { tc0.x, tc0.y, tc0.z });
    auto c100 = TexelFetchImage(*this, a_Image, { tc1.x, tc0.y, tc0.z });
    auto c010 = TexelFetchImage(*this, a_Image, { tc0.x, tc1.y, tc0.z });
    auto c110 = TexelFetchImage(*this, a_Image, { tc1.x, tc1.y, tc0.z });
    auto c001 = TexelFetchImage(*this, a_Image, { tc0.x, tc0.y, tc1.z });
    auto c101 = TexelFetchImage(*this, a_Image, { tc1.x, tc0.y, tc1.z });
    auto c011 = TexelFetchImage(*this, a_Image, { tc0.x, tc1.y, tc1.z });
    auto c111 = TexelFetchImage(*this, a_Image, { tc1.x, tc1.y, tc1.z });
    auto e    = Msg::PixelBilinearFilter(tcfr.x, tcfr.y, c000, c100, c010, c110);
    auto f    = Msg::PixelBilinearFilter(tcfr.x, tcfr.y, c001, c101, c011, c111);
    return glm::mix(e, f, tcfr.z);
}
glm::vec4 Msg::Sampler3D::Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod) const
{
    if (GetMipmapFilter() == Msg::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(a_Lod);
        return Sample(image, a_UV);
    }
    auto lodFract = glm::fract(a_Lod);
    auto& image0  = *a_Texture.at(glm::floor(a_Lod));
    auto color0   = Sample(image0, a_UV);
    auto& image1  = *a_Texture.at(glm::ceil(a_Lod));
    auto color1   = Sample(image1, a_UV);
    return glm::mix(color0, color1, lodFract);
}
glm::vec4 Msg::Sampler3D::TexelFetch(const Texture& a_Texture, const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const
{
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), a_TexelCoord);
}

// SamplerCube functions
enum class CubemapEdge {
    L, // Left
    R, // Right
    T, // Top
    B // Bottom
};
struct CubemapNeighbors {
    Msg::CubemapSide neighbor;
    glm::mat3x3 tcConv;
};
auto TCIdentity() { return glm::mat3x3(1); }
auto TCRotateCW() { return glm::rotate(TCIdentity(), float(std::numbers::pi / 2.f)); }
auto TCRotateCCW() { return glm::rotate(TCIdentity(), -float(std::numbers::pi / 2.f)); }
auto TCHalfTurn() { return TCRotateCW() * TCRotateCW(); }
auto TCInvertX() { return glm::translate(TCIdentity(), { -1, 1 }); }
auto TCInvertY() { return glm::translate(TCIdentity(), { 1, -1 }); }

CubemapNeighbors cubeNeighbors[6][4] {
    // PositiveX
    {
        { .neighbor = Msg::CubemapSide::PositiveZ, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::NegativeZ, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveY, .tcConv = TCRotateCW() },
        { .neighbor = Msg::CubemapSide::NegativeY, .tcConv = TCRotateCCW() },
    },
    // NegativeX
    {
        { .neighbor = Msg::CubemapSide::NegativeZ, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveZ, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveY, .tcConv = TCRotateCCW() },
        { .neighbor = Msg::CubemapSide::NegativeY, .tcConv = TCHalfTurn() },
    },
    // PositiveY
    {
        { .neighbor = Msg::CubemapSide::NegativeX, .tcConv = TCRotateCW() * TCInvertY() },
        { .neighbor = Msg::CubemapSide::PositiveX, .tcConv = TCRotateCW() * TCInvertY() },
        { .neighbor = Msg::CubemapSide::NegativeZ, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveZ, .tcConv = TCIdentity() },
    },
    // NegativeY
    {
        { .neighbor = Msg::CubemapSide::NegativeX, .tcConv = TCRotateCW() },
        { .neighbor = Msg::CubemapSide::PositiveX, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveZ, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::NegativeZ, .tcConv = TCIdentity() },
    },
    // PositiveZ
    {
        { .neighbor = Msg::CubemapSide::NegativeX, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveX, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveY, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::NegativeY, .tcConv = TCIdentity() },
    },
    // NegativeZ
    {
        { .neighbor = Msg::CubemapSide::PositiveX, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::NegativeX, .tcConv = TCIdentity() },
        { .neighbor = Msg::CubemapSide::PositiveY, .tcConv = TCHalfTurn() },
        { .neighbor = Msg::CubemapSide::NegativeY, .tcConv = TCHalfTurn() },
    }
};

glm::vec4 GetBorderCube(const Msg::Image& a_Image, const CubemapNeighbors& a_Neighbor, glm::ivec3 a_TexCoord)
{
    constexpr std::array<Msg::SamplerWrap, 3> wrapModes {
        Msg::SamplerWrap::Repeat,
        Msg::SamplerWrap::Repeat,
        Msg::SamplerWrap::Repeat
    };
    a_TexCoord   = a_Neighbor.tcConv * a_TexCoord;
    a_TexCoord.z = int(a_Neighbor.neighbor);
    a_TexCoord   = WrapTexelCoords(wrapModes, a_Image.GetSize(), a_TexCoord);
    return a_Image.Load(a_TexCoord);
}
glm::vec4 TexelFetchCubeNearest(const Msg::Image& a_Image, const glm::ivec3& a_TexCoord)
{
    constexpr std::array<Msg::SamplerWrap, 3> wrapModes {
        Msg::SamplerWrap::ClampToEdge,
        Msg::SamplerWrap::ClampToEdge,
        Msg::SamplerWrap::ClampToEdge
    };
    auto texCoord = WrapTexelCoords(wrapModes, a_Image.GetSize(), a_TexCoord);
    return a_Image.Load(texCoord);
}
glm::vec4 TexelFetchCube(const Msg::Image& a_Image, const glm::ivec3& a_TexCoord)
{
    constexpr std::array<Msg::SamplerWrap, 3> wrapModes {
        Msg::SamplerWrap::ClampToBorder,
        Msg::SamplerWrap::ClampToBorder,
        Msg::SamplerWrap::ClampToEdge
    };
    auto texCoord = WrapTexelCoords(wrapModes, a_Image.GetSize(), a_TexCoord);
    if (!IsClampedToBorder(a_Image, texCoord))
        return a_Image.Load(texCoord);
    glm::vec4 color(0.f);
    float samples   = 0;
    auto& neighbors = cubeNeighbors[texCoord.z];
    auto isLeft     = texCoord.x == -1;
    auto isRight    = texCoord.x == a_Image.GetSize().x;
    auto isTop      = texCoord.y == -1; // we're using DirectX-style cubemap
    auto isBottom   = texCoord.y == a_Image.GetSize().y;
    if (isLeft) {
        auto tc = glm::ivec3(
            texCoord.x,
            0,
            texCoord.z);
        color += GetBorderCube(a_Image, neighbors[int(CubemapEdge::L)], tc);
        samples++;
    } else if (isRight) {
        auto tc = glm::ivec3(
            texCoord.x,
            a_Image.GetSize().y - 1,
            texCoord.z);
        color += GetBorderCube(a_Image, neighbors[int(CubemapEdge::R)], tc);
        samples++;
    }
    if (isTop) {
        auto tc = glm::ivec3(
            0,
            texCoord.y,
            texCoord.z);
        color += GetBorderCube(a_Image, neighbors[int(CubemapEdge::T)], tc);
        samples++;
    } else if (isBottom) {
        auto tc = glm::ivec3(
            a_Image.GetSize().x - 1,
            texCoord.y,
            texCoord.z);
        color += GetBorderCube(a_Image, neighbors[int(CubemapEdge::B)], tc);
        samples++;
    }
    return color / samples;
}
glm::vec4 Msg::SamplerCube::Sample(const Image& a_Image, const glm::vec3& a_Dir) const
{
    auto uvw   = Msg::CubemapSampleDirToUVW(a_Dir);
    auto tcMax = glm::vec2(a_Image.GetSize() - 1u);
    auto tcf   = glm::vec2(uvw) * tcMax;
    if (GetImageFilter() == Msg::SamplerFilter::Nearest)
        TexelFetchCubeNearest(a_Image, { ManhattanRound(tcf), uvw.z });
    tcf -= 0.5f;
    auto tcfr = glm::fract(tcf);
    auto tc0  = tcf + 0.f;
    auto tc1  = tcf + 1.f;
    auto c00  = TexelFetchCube(a_Image, { tc0.x, tc0.y, uvw.z });
    auto c10  = TexelFetchCube(a_Image, { tc1.x, tc0.y, uvw.z });
    auto c01  = TexelFetchCube(a_Image, { tc0.x, tc1.y, uvw.z });
    auto c11  = TexelFetchCube(a_Image, { tc1.x, tc1.y, uvw.z });
    return Msg::PixelBilinearFilter(tcfr.x, tcfr.y, c00, c10, c01, c11);
}
glm::vec4 Msg::SamplerCube::Sample(const Texture& a_Texture, const glm::vec3& a_Dir, const float& a_Lod) const
{
    if (GetMipmapFilter() == Msg::SamplerFilter::Nearest) {
        auto& image = *a_Texture.at(a_Lod);
        return Sample(image, a_Dir);
    }
    auto lodFract = glm::fract(a_Lod);
    auto& image0  = *a_Texture.at(glm::floor(a_Lod));
    auto color0   = Sample(image0, a_Dir);
    auto& image1  = *a_Texture.at(glm::ceil(a_Lod));
    auto color1   = Sample(image1, a_Dir);
    return glm::mix(color0, color1, lodFract);
}

// Sampler1DArray functions
glm::vec4 Msg::Sampler1DArray::Sample(const Image& a_Image, const glm::vec2& a_UV) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y / (a_Image.GetSize().y - 1u), 0);
    return Sampler2D { *this }.Sample(a_Image, uvw);
}
glm::vec4 Msg::Sampler1DArray::Sample(const Texture& a_Texture, const glm::vec2& a_UV, const float& a_Lod) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y / (a_Texture.GetSize().y - 1u), 0);
    return Sampler2D { *this }.Sample(a_Texture, uvw, a_Lod);
}
glm::vec4 Msg::Sampler1DArray::TexelFetch(const Texture& a_Texture, const glm::ivec2& a_TexelCoord, const uint32_t& a_Lod) const
{
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), { a_TexelCoord, 0 });
}

// Sampler2DArray functions
glm::vec4 Msg::Sampler2DArray::Sample(const Image& a_Image, const glm::vec3& a_UV) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y, a_UV.z / (a_Image.GetSize().z - 1u));
    return Sampler3D { *this }.Sample(a_Image, uvw);
}
glm::vec4 Msg::Sampler2DArray::Sample(const Texture& a_Texture, const glm::vec3& a_UV, const float& a_Lod) const
{
    glm::vec3 uvw(a_UV.x, a_UV.y, a_UV.z / (a_Texture.GetSize().z - 1u));
    return Sampler3D { *this }.Sample(a_Texture, uvw, a_Lod);
}
glm::vec4 Msg::Sampler2DArray::TexelFetch(const Texture& a_Texture, const glm::ivec3& a_TexelCoord, const uint32_t& a_Lod) const
{
    return TexelFetchImage(*this, *a_Texture.at(a_Lod), a_TexelCoord);
}

glm::vec3 Msg::CubemapUVWToSampleVec(
    const glm::vec2& a_UV,
    const CubemapSide& a_Side)
{
    auto xyz = glm::vec3(0);
    // convert range 0 to 1 to -1 to 1
    auto uv = a_UV * 2.f - 1.f;
    switch (a_Side) {
    case CubemapSide::PositiveX:
        xyz = glm::vec3(1.0f, -uv.y, -uv.x);
        break;
    case CubemapSide::NegativeX:
        xyz = glm::vec3(-1.0f, -uv.y, uv.x);
        break;
    case CubemapSide::PositiveY:
        xyz = glm::vec3(uv.x, 1.0f, uv.y);
        break;
    case CubemapSide::NegativeY:
        xyz = glm::vec3(uv.x, -1.0f, -uv.y);
        break;
    case CubemapSide::PositiveZ:
        xyz = glm::vec3(uv.x, -uv.y, 1.0f);
        break;
    case CubemapSide::NegativeZ:
        xyz = glm::vec3(-uv.x, -uv.y, -1.0f);
        break;
    default:
        errorStream << "Incorrect side " << int(a_Side);
    }
    return normalize(xyz);
}

glm::vec3 Msg::CubemapUVWToSampleDir(const glm::vec3& a_UVW)
{
    return CubemapUVWToSampleVec(a_UVW, CubemapSide(a_UVW.z));
}

glm::vec3 Msg::CubemapSampleDirToUVW(const glm::vec3& a_UVW)
{
    auto& v        = a_UVW;
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
    return { uv * ma + 0.5f, faceIndex };
}