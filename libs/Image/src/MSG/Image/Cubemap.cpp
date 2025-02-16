#include <MSG/Buffer.hpp>
#include <MSG/Buffer/View.hpp>
#include <MSG/Image.hpp>
#include <MSG/Image/Cubemap.hpp>
#include <MSG/Image/ManhattanRound.hpp>
#include <MSG/Tools/Debug.hpp>
#include <MSG/Tools/ThreadPool.hpp>

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
        debugLog("Incorrect side");
    }
    return normalize(xyz);
}

glm::vec3 MSG::CubemapUVWToSampleVec(const glm::vec3& a_UVW)
{
    return CubemapUVWToSampleVec(a_UVW, CubemapSide(a_UVW.z));
}

glm::vec3 MSG::CubemapSampleVecToUVW(const glm::vec3& a_UVW)
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
    const size_t& a_Width, const size_t& a_Height,
    const Image& a_EquirectangularImage)
{
    Image cubemap(a_PixelDesc, a_Width, a_Height, 6);
    cubemap.Allocate();
    Tools::ThreadPool threadPool(6);
    for (auto side = 0u; side < 6; ++side) {
        threadPool.PushCommand([&cubemap, side, &a_EquirectangularImage]() mutable {
            auto image = cubemap.GetLayer(side);
            for (auto y = 0u; y < image.GetSize().y; ++y) {
                for (auto x = 0u; x < image.GetSize().x; ++x) {
                    const auto nx    = std::clamp((float)x / ((float)image.GetSize().x - 0.5f), 0.f, 1.f);
                    const auto ny    = std::clamp((float)y / ((float)image.GetSize().y - 0.5f), 0.f, 1.f);
                    const auto xyz   = CubemapUVWToSampleVec(glm::vec3(nx, ny, side));
                    const auto uv    = glm::vec3(CubemapSampleVecToEqui(xyz), 0);
                    const auto tc    = uv * glm::vec3(a_EquirectangularImage.GetSize());
                    const auto color = a_EquirectangularImage.Load(ManhattanRound(tc));
                    image.Store({ x, y, 0 }, color);
                }
            }
        },
            false);
    }
    return cubemap;
}

MSG::Image MSG::CubemapFromSides(const std::array<Image, 6>& a_Sides)
{
    Image cubemap(
        a_Sides.front().GetPixelDescriptor(),
        a_Sides.front().GetSize().x,
        a_Sides.front().GetSize().y,
        6);
    cubemap.Allocate();
    Tools::ThreadPool threadPool(6);
    for (auto sideIndex = 0u; sideIndex < 6; ++sideIndex) {
        cubemap.Allocate();
        threadPool.PushCommand([&src = a_Sides.at(sideIndex), dst = cubemap.GetLayer(sideIndex)]() mutable {
            src.Blit(dst, { 0, 0, 0 }, dst.GetSize());
        },
            false);
    }
    return cubemap;
}
