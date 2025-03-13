#include <MSG/Buffer.hpp>
#include <MSG/Buffer/View.hpp>
#include <MSG/Debug.hpp>
#include <MSG/Image.hpp>
#include <MSG/Image/Cubemap.hpp>
#include <MSG/Image/ManhattanRound.hpp>
#include <MSG/ThreadPool.hpp>

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

glm::vec3 MSG::CubemapUVWToSampleVec(
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

glm::vec3 MSG::CubemapUVWToSampleDir(const glm::vec3& a_UVW)
{
    return CubemapUVWToSampleVec(a_UVW, CubemapSide(a_UVW.z));
}

glm::vec3 MSG::CubemapSampleDirToUVW(const glm::vec3& a_UVW)
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

glm::vec2 MSG::CubemapSampleVecToEqui(glm::vec3 a_SampleVec)
{
    constexpr auto invAtan = glm::vec2(0.1591, 0.3183);
    auto uv                = glm::vec2(atan2(a_SampleVec.z, a_SampleVec.x), asin(a_SampleVec.y));
    uv *= invAtan;
    uv += 0.5;
    uv.y = 1 - uv.y;
    return uv;
}

MSG::Image MSG::CubemapFromEqui(
    const PixelDescriptor& a_PixelDesc,
    const uint32_t& a_Width, const uint32_t& a_Height,
    const Image& a_EquirectangularImage)
{
    ImageInfo cubeInfo {
        .width     = a_Width,
        .height    = a_Height,
        .depth     = 6,
        .pixelDesc = a_PixelDesc
    };
    Image cubemap(cubeInfo);
    cubemap.Allocate();
    ThreadPool threadPool(6);
    for (auto side = 0u; side < 6; ++side) {
        threadPool.PushCommand([&cubemap, side, &a_EquirectangularImage]() mutable {
            auto image = cubemap.GetLayer(side);
            for (auto y = 0u; y < image.GetSize().y; ++y) {
                for (auto x = 0u; x < image.GetSize().x; ++x) {
                    const auto nx    = std::clamp((float)x / ((float)image.GetSize().x - 0.5f), 0.f, 1.f);
                    const auto ny    = std::clamp((float)y / ((float)image.GetSize().y - 0.5f), 0.f, 1.f);
                    const auto xyz   = CubemapUVWToSampleDir(glm::vec3(nx, ny, side));
                    const auto uv    = glm::vec3(CubemapSampleVecToEqui(xyz), 0);
                    const auto coord = glm::clamp(
                        glm::uvec3(ManhattanRound(uv * glm::vec3(a_EquirectangularImage.GetSize()))),
                        glm::uvec3(0u), a_EquirectangularImage.GetSize() - 1u);
                    image.Store({ x, y, 0 }, a_EquirectangularImage.Load(coord));
                }
            }
        },
            false);
    }
    return cubemap;
}

MSG::Image MSG::CubemapFromSides(const std::array<Image, 6>& a_Sides)
{
    ImageInfo cubeInfo {
        .width     = a_Sides.front().GetSize().x,
        .height    = a_Sides.front().GetSize().y,
        .depth     = 6,
        .pixelDesc = a_Sides.front().GetPixelDescriptor()
    };
    Image cubemap(cubeInfo);
    cubemap.Allocate();
    ThreadPool threadPool(6);
    for (auto sideIndex = 0u; sideIndex < 6; ++sideIndex) {
        cubemap.Allocate();
        threadPool.PushCommand([&src = a_Sides.at(sideIndex), dst = cubemap.GetLayer(sideIndex)]() mutable {
            src.Blit(dst, { 0, 0, 0 }, dst.GetSize());
        },
            false);
    }
    return cubemap;
}
