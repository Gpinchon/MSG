#include <MSG/ImageUtils.hpp>
#include <MSG/PixelDescriptor.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/ThreadPool.hpp>

#include <glm/common.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>
#include <xutility>

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
    Image& a_EquirectangularImage)
{
    ImageInfo cubeInfo {
        .width     = a_Width,
        .height    = a_Height,
        .depth     = 6,
        .pixelDesc = a_PixelDesc
    };
    Sampler2D sampler;
    sampler.SetMinFilter(SamplerFilter::Linear);
    Image cubemap(cubeInfo);
    cubemap.Allocate();
    a_EquirectangularImage.Map();
    {
        ThreadPool threadPool(6);
        for (auto side = 0u; side < 6; ++side) {
            threadPool.PushCommand([&cubemap, &sampler, side, &a_EquirectangularImage]() mutable {
                auto image = ImageGetLayer(cubemap, side);
                image.Map();
                for (auto y = 0u; y < image.GetSize().y; ++y) {
                    for (auto x = 0u; x < image.GetSize().x; ++x) {
                        const auto nx  = std::clamp((float)x / ((float)image.GetSize().x - 0.5f), 0.f, 1.f);
                        const auto ny  = std::clamp((float)y / ((float)image.GetSize().y - 0.5f), 0.f, 1.f);
                        const auto xyz = CubemapUVWToSampleDir(glm::vec3(nx, ny, side));
                        const auto uv  = glm::vec3(CubemapSampleVecToEqui(xyz), 0);
                        image.Store({ x, y, 0 }, sampler.Sample(a_EquirectangularImage, uv));
                    }
                }
                image.Unmap();
            },
                false);
        }
    }
    a_EquirectangularImage.Unmap();
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
        threadPool.PushCommand([&src = a_Sides.at(sideIndex), dst = ImageGetLayer(cubemap, sideIndex)]() mutable {
            ImageBlit(src, dst, { 0, 0, 0 }, dst.GetSize());
        },
            false);
    }
    return cubemap;
}

MSG::Image MSG::ImageGetLayer(const Image& a_Src, const uint32_t& a_Layer)
{
    return ImageInfo {
        .width     = a_Src.GetSize().x,
        .height    = a_Src.GetSize().y,
        .pixelDesc = a_Src.GetPixelDescriptor(),
        .storage   = { a_Src.GetStorage(), a_Layer }
    };
}

void MSG::ImageBlit(
    const Image& a_Src,
    Image& a_Dst,
    const glm::uvec3& a_Offset,
    const glm::uvec3& a_Size)
{
    a_Src.Map();
    a_Dst.Map();
    glm::uvec3 endPixel = a_Offset + a_Size;
    for (auto z = a_Offset.z; z < endPixel.z; z++) {
        auto w = z / float(endPixel.z);
        for (auto y = a_Offset.y; y < endPixel.y; y++) {
            auto v = y / float(endPixel.y);
            for (auto x = a_Offset.x; x < endPixel.x; x++) {
                auto u     = x / float(endPixel.x);
                auto UVW   = glm::vec3(u, v, w);
                auto dstTc = UVW * glm::vec3(a_Dst.GetSize());
                a_Dst.Store(dstTc, a_Src.Load({ x, y, z }));
            }
        }
    }
    a_Dst.Unmap();
    a_Src.Unmap();
}

MSG::Image MSG::ImageCopy(const Image& a_Src)
{
    Image newImage(a_Src);
    newImage.Allocate();
    ImageBlit(a_Src, newImage, { 0, 0, 0 }, a_Src.GetSize());
    return newImage;
}

void MSG::ImageFill(Image& a_Dst, const PixelColor& a_Color)
{
    std::vector<std::byte> pixels(a_Dst.GetSize().x * a_Dst.GetSize().y * a_Dst.GetSize().z * a_Dst.GetPixelDescriptor().GetPixelSize());
    a_Dst.GetPixelDescriptor().SetColorToBytesRange(
        std::to_address(pixels.begin()),
        std::to_address(pixels.end()),
        a_Color);
    a_Dst.GetStorage().Write(a_Dst.GetSize(), a_Dst.GetPixelDescriptor(), glm::uvec3(0u), a_Dst.GetSize(), std::move(pixels));
}

void MSG::ImageResize(Image& a_Dst, const glm::uvec3& a_NewSize)
{
    if (a_NewSize == a_Dst.GetSize())
        return;
    auto newImage = Image(
        ImageInfo {
            .width     = a_NewSize.x,
            .height    = a_NewSize.y,
            .depth     = a_NewSize.z,
            .pixelDesc = a_Dst.GetPixelDescriptor(),
        });
    newImage.Allocate();
    newImage.Map();
    a_Dst.Map();
    for (uint32_t z = 0; z < a_NewSize.z; z++) {
        uint32_t tcZ = z / float(a_NewSize.z) * a_Dst.GetSize().z;
        for (uint32_t y = 0; y < a_NewSize.y; y++) {
            uint32_t tcY = y / float(a_NewSize.y) * a_Dst.GetSize().y;
            for (uint32_t x = 0; x < a_NewSize.x; x++) {
                uint32_t tcX = x / float(a_NewSize.x) * a_Dst.GetSize().x;
                newImage.Store({ x, y, z }, a_Dst.Load({ tcX, tcY, tcZ }));
            }
        }
    }
    a_Dst.Unmap();
    newImage.Unmap();
    a_Dst = newImage;
}

void MSG::ImageFlipX(Image& a_Dst)
{
    a_Dst.Map();
    for (auto z = 0u; z < a_Dst.GetSize().z; z++) {
        for (auto y = 0u; y < a_Dst.GetSize().y; y++) {
            for (auto x = 0u; x < a_Dst.GetSize().x / 2; x++) {
                auto x1   = a_Dst.GetSize().x - (x + 1);
                auto temp = a_Dst.Load({ x, y, z });
                a_Dst.Store({ x, y, z }, a_Dst.Load({ x1, y, z }));
                a_Dst.Store({ x1, y, z }, temp);
            }
        }
    }
    a_Dst.Unmap();
}

void MSG::ImageFlipY(Image& a_Dst)
{
    a_Dst.Map();
    for (auto z = 0u; z < a_Dst.GetSize().z; z++) {
        for (auto y = 0u; y < a_Dst.GetSize().y / 2; y++) {
            auto y1 = a_Dst.GetSize().y - (y + 1);
            for (auto x = 0u; x < a_Dst.GetSize().x; x++) {
                auto temp = a_Dst.Load({ x, y, z });
                a_Dst.Store({ x, y, z }, a_Dst.Load({ x, y1, z }));
                a_Dst.Store({ x, y1, z }, temp);
            }
        }
    }
    a_Dst.Unmap();
}

void MSG::ImageFlipZ(Image& a_Dst)
{
    a_Dst.Map();
    for (auto z = 0u; z < a_Dst.GetSize().z / 2; z++) {
        auto z1 = a_Dst.GetSize().z - (z + 1);
        for (auto y = 0u; y < a_Dst.GetSize().y; y++) {
            for (auto x = 0u; x < a_Dst.GetSize().x; x++) {
                auto temp = a_Dst.Load({ x, y, z });
                a_Dst.Store({ x, y, z }, a_Dst.Load({ x, y, z1 }));
                a_Dst.Store({ x, y, z1 }, temp);
            }
        }
    }
    a_Dst.Unmap();
}

void MSG::ImageApplyTransform(Image& a_Dst, const glm::mat3x3& a_TexCoordTransform)
{
    a_Dst.Map();
    auto tempImg = ImageCopy(a_Dst);
    for (auto z = 0u; z < a_Dst.GetSize().z; z++) {
        for (auto y = 0u; y < a_Dst.GetSize().y; y++) {
            for (auto x = 0u; x < a_Dst.GetSize().x; x++) {
                auto newCoord = a_TexCoordTransform * glm::vec3(x, y, z);
                newCoord      = glm::clamp(newCoord, { 0, 0, 0 }, glm::vec3(a_Dst.GetSize()) - 1.f);
                a_Dst.Store(newCoord, tempImg.Load({ x, y, z }));
            }
        }
    }
    a_Dst.Unmap();
}
