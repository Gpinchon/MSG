#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Image/Image2D.hpp>
#include <Tools/Debug.hpp>
#include <Tools/ThreadPool.hpp>

#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

namespace TabGraph::SG {
glm::vec3 Cubemap::UVToXYZ(const CubemapSide& a_Side, const glm::vec2& a_UV)
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

glm::vec2 XYZToEquirectangular(glm::vec3 xyz)
{
    constexpr auto invAtan = glm::vec2(0.1591, 0.3183);
    auto uv                = glm::vec2(atan2(xyz.z, xyz.x), asin(xyz.y));
    uv *= invAtan;
    uv += 0.5;
    uv.y = 1 - uv.y;
    return uv;
}

glm::vec3 GetImageUV(const glm::vec3 v)
{
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

Cubemap::Cubemap(
    const Pixel::Description& a_PixelDesc,
    const size_t& a_Width, const size_t& a_Height,
    const std::shared_ptr<BufferView>& a_BufferView)
    : Inherit(a_PixelDesc, a_Width, a_Height, 6, a_BufferView)
{
    if (!GetBufferAccessor().empty())
        UpdateSides();
}

Cubemap::Cubemap(
    const Pixel::Description& a_PixelDesc,
    const size_t& a_Width, const size_t& a_Height,
    const Image2D& a_EquirectangularImage)
    : Cubemap(a_PixelDesc, a_Width, a_Height)
{
    Cubemap::Allocate();
    Tools::ThreadPool threadPool(6);
    for (auto side = 0u; side < 6; ++side) {
        threadPool.PushCommand([this, side, a_EquirectangularImage]() mutable {
            auto& image = at(side);
            for (auto y = 0u; y < image.GetSize().y; ++y) {
                for (auto x = 0u; x < image.GetSize().x; ++x) {
                    const auto nx    = std::clamp((float)x / ((float)image.GetSize().x - 0.5f), 0.f, 1.f);
                    const auto ny    = std::clamp((float)y / ((float)image.GetSize().y - 0.5f), 0.f, 1.f);
                    const auto xyz   = UVToXYZ(CubemapSide(side), glm::vec2(nx, ny));
                    const auto uv    = glm::vec3(XYZToEquirectangular(xyz), 0);
                    const auto color = a_EquirectangularImage.LoadNorm(uv, ImageFilter::Bilinear);
                    image.Store({ x, y, 0 }, color);
                }
            }
        },
            false);
    }
    threadPool.Wait();
}

void Cubemap::Allocate()
{
    Image::Allocate();
    UpdateSides();
}

Pixel::Color Cubemap::LoadNorm(
    const glm::vec3& a_Coords,
    const ImageFilter& a_Filter) const
{
    const auto imageUV = GetImageUV(a_Coords);
    return at(int(imageUV.z)).LoadNorm({ imageUV.x, imageUV.y, 0 }, a_Filter);
}

void Cubemap::StoreNorm(
    const glm::vec3& a_Coords,
    const Pixel::Color& a_Color)
{
    const auto imageUV = GetImageUV(a_Coords);
    return at(int(imageUV.z)).StoreNorm({ imageUV.x, imageUV.y, 0 }, a_Color);
}

void Cubemap::UpdateSides()
{
    const auto textureByteSize = GetPixelDescription().GetSize() * GetSize().x * GetSize().y;
    for (uint32_t side = 0; side < 6; ++side) {
        const auto bufferView = std::make_shared<BufferView>(GetBufferAccessor().GetBufferView()->GetBuffer(), textureByteSize * side, textureByteSize);
        at(side)              = Image2D(GetPixelDescription(), GetSize().x, GetSize().y, bufferView);
    }
}
}
