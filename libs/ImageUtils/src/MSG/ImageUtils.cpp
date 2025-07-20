#include <MSG/Debug.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/PixelDescriptor.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/ThreadPool.hpp>

#include <glm/common.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

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
            ImageBlit(src, dst, { 0, 0, 0 }, { 0, 0, 0 }, dst.GetSize());
        },
            false);
    }
    return cubemap;
}

MSG::Image MSG::ImageCompress(const Image& a_Src)
{
    auto inputSize     = a_Src.GetSize();
    PixelDescriptor pd = PixelSizedFormat::DXT5_RGBA;
    auto newImage      = Image({
             .width     = inputSize.x,
             .height    = inputSize.y,
             .depth     = inputSize.z,
             .pixelDesc = pd,
    });
    newImage.Allocate();
    ImageClear(newImage);
    a_Src.Map();
    constexpr glm::uvec3 blockSize = { 4, 4, 1 };
    constexpr size_t blockByteSize = 16;
    auto blockCount                = (inputSize + (blockSize - 1u)) / blockSize;
    std::vector<std::byte> compressedData(pd.GetPixelBufferByteSize(inputSize), std::byte(0));
    for (uint32_t blockZ = 0; blockZ < blockCount.z; blockZ++) {
        for (uint32_t blockY = 0; blockY < blockCount.y; blockY++) {
            for (uint32_t blockX = 0; blockX < blockCount.x; blockX++) {
                auto blockCoords = glm::uvec3 { blockX, blockY, blockZ } * blockSize;
                auto decompSize  = glm::min(blockSize, inputSize - blockCoords);
                std::array<glm::vec4, 16> colors;
                colors.fill(glm::vec4(0));
                for (uint32_t z = 0; z < decompSize.z; z++) {
                    for (uint32_t y = 0; y < decompSize.y; y++) {
                        for (uint32_t x = 0; x < decompSize.x; x++) {
                            auto colIndex    = (z * blockSize.x * blockSize.y) + (y * blockSize.x) + x;
                            colors[colIndex] = a_Src.Load(blockCoords + glm::uvec3 { x, y, z });
                        }
                    }
                }
                auto block      = newImage.GetPixelDescriptor().CompressBlock(colors.data());
                auto blockIndex = newImage.GetPixelDescriptor().GetPixelIndex(inputSize, blockCoords);
                std::copy(block.begin(), block.end(), compressedData.begin() + blockIndex);
            }
        }
    }
    a_Src.Unmap();
    newImage.Write(std::move(compressedData));
    return newImage;
}

MSG::Image MSG::ImageDecompress(const Image& a_Src)
{
    auto inputSize     = a_Src.GetSize();
    PixelDescriptor pd = PixelSizedFormat::Uint8_NormalizedRGBA;
    auto newImage      = Image({
             .width     = inputSize.x,
             .height    = inputSize.y,
             .depth     = inputSize.z,
             .pixelDesc = pd,
    });
    newImage.Allocate();
    auto blockSize  = glm::uvec3(4, 4, 1);
    auto blockCount = (inputSize + (blockSize - 1u)) / blockSize;
    a_Src.Map();
    newImage.Map();
    for (uint32_t blockZ = 0; blockZ < blockCount.z; blockZ++) {
        for (uint32_t blockY = 0; blockY < blockCount.y; blockY++) {
            for (uint32_t blockX = 0; blockX < blockCount.x; blockX++) {
                auto blockCoords = glm::uvec3 { blockX, blockY, blockZ } * blockSize;
                auto colors      = a_Src.GetPixelDescriptor().DecompressBlock(a_Src.Read(blockCoords, blockSize).data());
                auto decompSize  = glm::min(blockSize, inputSize - blockCoords);
                for (uint32_t z = 0; z < decompSize.z; z++) {
                    for (uint32_t y = 0; y < decompSize.y; y++) {
                        for (uint32_t x = 0; x < decompSize.x; x++) {
                            auto colIndex = (z * blockSize.x * blockSize.y) + (y * blockSize.x) + x;
                            newImage.Store(blockCoords + glm::uvec3 { x, y, z }, colors[colIndex]);
                        }
                    }
                }
            }
        }
    }
    newImage.Unmap();
    a_Src.Unmap();
    return newImage;
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
    const glm::uvec3& a_SrcOffset,
    const glm::uvec3& a_DstOffset,
    const glm::uvec3& a_Size)
{
    const auto srcEnd = a_SrcOffset + a_Size;
    const auto dstEnd = a_DstOffset + a_Size;
    const auto extent = a_Size;
    assert(glm::all(glm::lessThanEqual(srcEnd, a_Src.GetSize())));
    assert(glm::all(glm::lessThanEqual(dstEnd, a_Dst.GetSize())));
    a_Src.Map();
    a_Dst.Map();
    for (auto z = 0; z < extent.z; z++) {
        for (auto y = 0; y < extent.y; y++) {
            for (auto x = 0; x < extent.x; x++) {
                const auto coord    = glm::uvec3(x, y, z);
                const auto srcCoord = a_SrcOffset + coord;
                const auto dstCoord = a_DstOffset + coord;
                a_Dst.Store(dstCoord, a_Src.Load(srcCoord));
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
    ImageBlit(a_Src, newImage, { 0, 0, 0 }, { 0, 0, 0 }, a_Src.GetSize());
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

void MSG::ImageClear(Image& a_Dst)
{
    a_Dst.GetStorage().Clear(a_Dst.GetSize(), a_Dst.GetPixelDescriptor());
}

MSG::Image MSG::ImageResize(const Image& a_Src, const glm::uvec3& a_NewSize)
{
    if (a_NewSize == a_Src.GetSize())
        return ImageCopy(a_Src);
    auto newImage = Image(
        ImageInfo {
            .width     = a_NewSize.x,
            .height    = a_NewSize.y,
            .depth     = a_NewSize.z,
            .pixelDesc = a_Src.GetPixelDescriptor(),
        });
    newImage.Allocate();
    if (a_Src.GetPixelDescriptor().GetSizedFormat() == MSG::PixelSizedFormat::DXT5_RGBA)
        ImageClear(newImage);
    newImage.Map();
    a_Src.Map();
    for (uint32_t z = 0; z < a_NewSize.z; z++) {
        uint32_t tcZ = z / float(a_NewSize.z) * a_Src.GetSize().z;
        for (uint32_t y = 0; y < a_NewSize.y; y++) {
            uint32_t tcY = y / float(a_NewSize.y) * a_Src.GetSize().y;
            for (uint32_t x = 0; x < a_NewSize.x; x++) {
                uint32_t tcX = x / float(a_NewSize.x) * a_Src.GetSize().x;
                auto col     = a_Src.Load({ tcX, tcY, tcZ });
                newImage.Store({ x, y, z }, col);
            }
        }
    }
    a_Src.Unmap();
    newImage.Unmap();
    return newImage;
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
