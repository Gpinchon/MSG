#include <MSG/Debug.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/PixelDescriptor.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/ThreadPool.hpp>

#include <glm/common.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

#include <span>

glm::vec2 Msg::CubemapSampleVecToEqui(glm::vec3 a_SampleVec)
{
    constexpr auto invAtan = glm::vec2(0.1591, 0.3183);
    auto uv                = glm::vec2(atan2(a_SampleVec.z, a_SampleVec.x), asin(a_SampleVec.y));
    uv *= invAtan;
    uv += 0.5;
    uv.y = 1 - uv.y;
    return uv;
}

Msg::Image Msg::CubemapFromEqui(
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

Msg::Image Msg::CubemapFromSides(const std::array<Image, 6>& a_Sides)
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

Msg::Image Msg::ImageCompress(const Image& a_Src)
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

Msg::Image Msg::ImageDecompress(const Image& a_Src)
{
    constexpr glm::uvec3 blockSize(4, 4, 1);
    auto inputSize     = a_Src.GetSize();
    auto blockCount    = (inputSize + (blockSize - 1u)) / blockSize;
    PixelDescriptor pd = PixelSizedFormat::Uint8_NormalizedRGBA;
    auto newImage      = Image({
             .width     = inputSize.x,
             .height    = inputSize.y,
             .depth     = inputSize.z,
             .pixelDesc = pd,
    });
    newImage.Allocate();
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

std::vector<std::byte> Msg::ImageDecompress(const Image& a_Src, const glm::uvec3& a_Offset, const glm::uvec3& a_Size)
{
    constexpr glm::uvec3 blockSize(4, 4, 1);
    constexpr size_t blockByteSize = 16;
    PixelDescriptor pd(PixelSizedFormat::Uint8_NormalizedRGBA);

    glm::uvec3 blockStart    = a_Offset / blockSize;
    glm::uvec3 blockEnd      = glm::ceil(glm::vec3(a_Offset + a_Size) / glm::vec3(blockSize));
    glm::uvec3 blockCount    = blockEnd - blockStart;
    glm::uvec3 srcPixelStart = blockStart * blockSize;
    glm::uvec3 srcPixelSize  = blockCount * blockSize;
    std::vector<glm::u8vec4> decompressedBlocks(blockCount.x * blockCount.y * blockCount.z * 16);
    for (uint32_t blockZ = blockStart.z; blockZ < blockEnd.z; blockZ++) {
        for (uint32_t blockY = blockStart.y; blockY < blockEnd.y; blockY++) {
            for (uint32_t blockX = blockStart.x; blockX < blockEnd.x; blockX++) {
                auto srcPixelCoords   = glm::uvec3 { blockX, blockY, blockZ } * blockSize;
                auto colors           = a_Src.GetPixelDescriptor().DecompressBlockToUI8(a_Src.Read(srcPixelCoords, blockSize).data());
                glm::uvec3 decompSize = glm::min(blockSize, (a_Offset + a_Size) - srcPixelCoords);
                for (uint32_t pixelZ = 0; pixelZ < decompSize.z; pixelZ++) {
                    for (uint32_t pixelY = 0; pixelY < decompSize.y; pixelY++) {
                        for (uint32_t pixelX = 0; pixelX < decompSize.x; pixelX++) {
                            auto colIndex               = (pixelZ * blockSize.x * blockSize.y) + (pixelY * blockSize.x) + pixelX;
                            auto srcPixel               = (srcPixelCoords - srcPixelStart) + glm::uvec3 { pixelX, pixelY, pixelZ };
                            uint32_t srcPixI            = pd.GetPixelIndex(srcPixelSize, srcPixel) / pd.GetPixelSize();
                            decompressedBlocks[srcPixI] = colors[colIndex];
                        }
                    }
                }
            }
        }
    }
    glm::uvec3 srcOffset(a_Offset - srcPixelStart);
    std::vector<std::byte> ret(a_Size.x * a_Size.y * a_Size.z * sizeof(glm::u8vec4));
    std::span<glm::u8vec4> retVec4(reinterpret_cast<glm::u8vec4*>(ret.data()), a_Size.x * a_Size.y * a_Size.z);
    for (uint32_t dstZ = 0; dstZ < a_Size.z; dstZ++) {
        for (uint32_t dstY = 0; dstY < a_Size.y; dstY++) {
            for (uint32_t dstX = 0; dstX < a_Size.x; dstX++) {
                glm::uvec3 dstPixel = { dstX, dstY, dstZ };
                glm::uvec3 srcPixel = dstPixel + srcOffset;
                uint32_t dstPixI    = pd.GetPixelIndex(a_Size, dstPixel) / pd.GetPixelSize();
                uint32_t srcPixI    = pd.GetPixelIndex(srcPixelSize, srcPixel) / pd.GetPixelSize();
                retVec4[dstPixI]    = decompressedBlocks[srcPixI];
            }
        }
    }
    return ret;
}

Msg::Image Msg::ImageGetLayer(const Image& a_Src, const uint32_t& a_Layer)
{
    return ImageInfo {
        .width     = a_Src.GetSize().x,
        .height    = a_Src.GetSize().y,
        .pixelDesc = a_Src.GetPixelDescriptor(),
        .storage   = { a_Src.GetStorage(), a_Layer }
    };
}

void Msg::ImageBlit(
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

Msg::Image Msg::ImageCopy(const Image& a_Src)
{
    Image newImage(a_Src);
    newImage.Allocate();
    ImageBlit(a_Src, newImage, { 0, 0, 0 }, { 0, 0, 0 }, a_Src.GetSize());
    return newImage;
}

void Msg::ImageFill(Image& a_Dst, const PixelColor& a_Color)
{
    std::vector<std::byte> pixels(a_Dst.GetSize().x * a_Dst.GetSize().y * a_Dst.GetSize().z * a_Dst.GetPixelDescriptor().GetPixelSize());
    a_Dst.GetPixelDescriptor().SetColorToBytesRange(
        std::to_address(pixels.begin()),
        std::to_address(pixels.end()),
        a_Color);
    a_Dst.GetStorage().Write(a_Dst.GetSize(), a_Dst.GetPixelDescriptor(), glm::uvec3(0u), a_Dst.GetSize(), std::move(pixels));
}

void Msg::ImageClear(Image& a_Dst)
{
    a_Dst.GetStorage().Clear(a_Dst.GetSize(), a_Dst.GetPixelDescriptor());
}

Msg::Image Msg::ImageResize(const Image& a_Src, const glm::uvec3& a_NewSize)
{
    if (a_NewSize == a_Src.GetSize())
        return ImageCopy(a_Src);
    ImageInfo info;
    info.width     = a_NewSize.x;
    info.height    = a_NewSize.y;
    info.depth     = a_NewSize.z;
    info.pixelDesc = a_Src.GetPixelDescriptor();
    info.storage   = ImageResize(a_Src.Read(), a_Src.GetPixelDescriptor(), a_Src.GetSize(), a_NewSize);
    return Image(info);
}

std::vector<std::byte> Msg::ImageResize(
    const std::vector<std::byte>& a_Src, const PixelDescriptor& a_PixDsc,
    const glm::uvec3& a_SrcSize, const glm::uvec3& a_DstSize)
{
    if (a_SrcSize == a_DstSize)
        return a_Src;
    assert(!a_PixDsc.IsCompressed() && "Decompress before resizing !");
    std::vector<std::byte> result(a_PixDsc.GetPixelBufferByteSize(a_DstSize));
    size_t pixByteSize = a_PixDsc.GetPixelSize();
    for (uint32_t z = 0; z < a_DstSize.z; z++) {
        uint32_t tcZ = z / float(a_DstSize.z) * a_SrcSize.z;
        for (uint32_t y = 0; y < a_DstSize.y; y++) {
            uint32_t tcY = y / float(a_DstSize.y) * a_SrcSize.y;
            for (uint32_t x = 0; x < a_DstSize.x; x++) {
                uint32_t tcX = x / float(a_DstSize.x) * a_SrcSize.x;
                glm::uvec3 srcCoord(tcX, tcY, tcZ);
                glm::uvec3 dstCoord(x, y, z);
                size_t srcByteIndex = a_PixDsc.GetPixelIndex(a_SrcSize, srcCoord);
                size_t dstByteIndex = a_PixDsc.GetPixelIndex(a_DstSize, dstCoord);
                std::memcpy(&result[dstByteIndex], &a_Src[srcByteIndex], pixByteSize);
            }
        }
    }
    return result;
}

Msg::Image Msg::ImageConvert(const Image& a_Src, const PixelDescriptor& a_PixelDesc)
{
    ImageInfo info;
    info.width     = a_Src.GetSize().x;
    info.height    = a_Src.GetSize().y;
    info.depth     = a_Src.GetSize().z;
    info.pixelDesc = a_PixelDesc;
    info.storage   = ImageConvert(
        a_Src.Read(), a_Src.GetPixelDescriptor(), a_Src.GetSize(),
        a_PixelDesc);
    return Msg::Image(info);
}

std::vector<std::byte> Msg::ImageConvert(
    const std::vector<std::byte>& a_Src, const PixelDescriptor& a_SrcPixDsc, const glm::uvec3& a_SrcSize,
    const PixelDescriptor& a_DstPixDsc)
{
    std::vector<std::byte> result(a_DstPixDsc.GetPixelBufferByteSize(a_SrcSize));
    for (uint32_t z = 0; z < a_SrcSize.z; z++) {
        for (uint32_t y = 0; y < a_SrcSize.y; y++) {
            for (uint32_t x = 0; x < a_SrcSize.x; x++) {
                glm::uvec3 pixelCoord(x, y, z);
                size_t srcByteIndex = a_SrcPixDsc.GetPixelIndex(a_SrcSize, pixelCoord);
                size_t dstByteIndex = a_DstPixDsc.GetPixelIndex(a_SrcSize, pixelCoord);
                auto srcBytes       = &a_Src[srcByteIndex];
                auto dstBytes       = &result[dstByteIndex];
                a_DstPixDsc.SetColorToBytes(
                    dstBytes,
                    a_SrcPixDsc.GetColorFromBytes(srcBytes));
            }
        }
    }
    return result;
}

void Msg::ImageFlipX(Image& a_Dst)
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

void Msg::ImageFlipY(Image& a_Dst)
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

void Msg::ImageFlipZ(Image& a_Dst)
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

void Msg::ImageApplyTransform(Image& a_Dst, const glm::mat3x3& a_TexCoordTransform)
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
