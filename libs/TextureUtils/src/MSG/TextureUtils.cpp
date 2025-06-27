#include <MSG/Debug.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/TextureUtils.hpp>
#include <MSG/ThreadPool.hpp>

#include <cmath>

#include <FasTC/CompressedImage.h>
#include <FasTC/CompressionFormat.h>
#include <FasTC/Image.h>
#include <FasTC/TexComp.h>

#include <glm/common.hpp>
#include <glm/vec2.hpp>

namespace MSG {
uint32_t GetMipCount(const int32_t& a_BaseSize)
{
    return a_BaseSize == 0 ? 0 : floor(log2(a_BaseSize));
}

template <typename T>
uint32_t GetMipCount(const T& a_BaseSize)
{
    uint32_t maxCount = 0;
    for (uint8_t i = 0; i < T::length(); i++)
        maxCount = std::max(GetMipCount(a_BaseSize[i]), maxCount);
    return maxCount;
}

template <uint8_t Dimension>
auto CreateMip(const PixelDescriptor& a_PD, const glm::ivec3& a_BaseSize, const glm::ivec3& a_LevelSize)
{
    glm::uvec3 size = a_BaseSize;
    for (uint8_t i = 0; i < Dimension; ++i)
        size[i] = a_LevelSize[i];
    return std::make_shared<Image>(ImageInfo {
        .width     = size.x,
        .height    = size.y,
        .depth     = size.z,
        .pixelDesc = a_PD,
    });
}

template <typename SamplerType>
void LvlGenFunc(ThreadPool& a_Tp, const SamplerType& a_Sampler, const uint32_t& a_LvlIndex, const Image& a_Src, Image& a_Dst)
{
    auto tcMax = glm::vec3(a_Dst.GetSize() - 1u);
    for (uint32_t z = 0; z < a_Dst.GetSize().z; z++) {
        float w = float(z + 0.25f) / tcMax.z; // + 0.25f and not 0.5f because (no idea)...
        for (uint32_t y = 0; y < a_Dst.GetSize().y; y++) {
            float v = float(y + 0.25f) / tcMax.y;
            for (uint32_t x = 0; x < a_Dst.GetSize().x; x++) {
                float u = float(x + 0.25f) / tcMax.x;
                a_Dst.Store({ x, y, z }, a_Sampler.Sample(a_Src, glm::vec3(u, v, w)));
            }
        }
    }
}

template <>
void LvlGenFunc(ThreadPool& a_Tp, const SamplerCube& a_Sampler, const uint32_t& a_LvlIndex, const Image& a_Src, Image& a_Dst)
{
    auto tcMax = glm::vec2(a_Dst.GetSize() - 1u);
    for (auto side = 0u; side < 6; side++) {
        a_Tp.PushCommand([&a_Sampler, &a_Src, &a_Dst, tcMax, side]() mutable {
            for (uint32_t y = 0u; y < a_Dst.GetSize().y; y++) {
                auto v = (y + 0.25f) / tcMax.y;
                for (uint32_t x = 0u; x < a_Dst.GetSize().x; x++) {
                    auto u     = (x + 0.25f) / tcMax.x;
                    auto dir   = CubemapUVWToSampleDir({ u, v, side });
                    auto color = a_Sampler.Sample(a_Src, dir);
                    a_Dst.Store({ x, y, side }, color);
                }
            }
        },
            false);
    }
    a_Tp.Wait();
}

template <uint8_t Dimension, typename SamplerType>
void GenerateMipMaps(Texture& a_Texture, const SamplerType& a_Sampler = {})
{
    ThreadPool threadPool;
    const auto pixelDesc = a_Texture.GetPixelDescriptor();
    const auto baseSize  = glm::ivec3(a_Texture.GetSize());
    const auto mipNbr    = GetMipCount(baseSize) + 1;
    auto srcLevel        = a_Texture.front();
    srcLevel->Map();
    a_Texture.resize(mipNbr);
    for (auto level = 1u; level < mipNbr; level++) {
        auto levelSize      = glm::max(baseSize / int(pow(2, level)), 1);
        auto mip            = CreateMip<Dimension>(pixelDesc, baseSize, levelSize);
        a_Texture.at(level) = mip;
        mip->Allocate();
        mip->Map();
        LvlGenFunc(threadPool, a_Sampler, level, *srcLevel, *mip);
        srcLevel->Unmap();
        srcLevel = mip;
    }
    srcLevel->Unmap();
}

auto Compress2D(Image&& a_Image, const uint8_t& a_Quality)
{
    auto inputSize = a_Image.GetSize();
    if (a_Image.GetPixelDescriptor().GetSizedFormat() != PixelSizedFormat::Uint8_NormalizedRGBA) {
        debugLog("Image is not Uint8_NormalizedRGBA, creating properly sized image");
        auto newImage = Image({
            .width     = inputSize.x,
            .height    = inputSize.y,
            .pixelDesc = PixelSizedFormat::Uint8_NormalizedRGBA,
        });
        newImage.Allocate();
        ImageBlit(a_Image, newImage, { 0u, 0u, 0u }, a_Image.GetSize());
        a_Image = newImage;
    }
    SCompressionSettings settings;
    settings.format   = FasTC::eCompressionFormat_DXT5;
    settings.iQuality = a_Quality;
    FasTC::Image<FasTC::Pixel> image(inputSize.x, inputSize.y, reinterpret_cast<const uint32_t*>(std::to_address(a_Image.Read().begin())));
    std::unique_ptr<CompressedImage> compressedImage(CompressImage(&image, settings));
    PixelDescriptor pd = PixelSizedFormat::DXT5_RGBA;
    assert(pd.GetPixelBufferByteSize(inputSize) == compressedImage->GetCompressedSize()); // sanity check
    auto newImage = std::make_shared<Image>(ImageInfo {
        .width     = inputSize.x,
        .height    = inputSize.y,
        .pixelDesc = pd,
    });
    newImage->Allocate();
    newImage->Write({
        reinterpret_cast<const std::byte*>(compressedImage->GetCompressedData()),
        reinterpret_cast<const std::byte*>(compressedImage->GetCompressedData()) + compressedImage->GetCompressedSize(),
    });
    return newImage;
}
}

void MSG::TextureGenerateMipmaps(Texture& a_Dst)
{
    Sampler samplerSettings;
    samplerSettings.SetMinFilter(SamplerFilter::LinearMipmapLinear);
    if (a_Dst.GetType() == TextureType::Texture1D)
        GenerateMipMaps<1, Sampler1D>(a_Dst, samplerSettings);
    else if (a_Dst.GetType() == TextureType::Texture2D)
        GenerateMipMaps<2, Sampler2D>(a_Dst, samplerSettings);
    else if (a_Dst.GetType() == TextureType::Texture3D)
        GenerateMipMaps<3, Sampler3D>(a_Dst, samplerSettings);
    else if (a_Dst.GetType() == TextureType::TextureCubemap)
        GenerateMipMaps<2, SamplerCube>(a_Dst, samplerSettings);
    else
        errorLog("Mipmap generation not implemented for this texture type yet");
}

void MSG::TextureCompress(Texture& a_Dst, const uint8_t& a_Quality)
{
    if (a_Dst.GetPixelDescriptor().GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        debugLog("Texture already compressed");
        return;
    }
    if (a_Dst.GetType() == TextureType::Texture2D) {
        TextureBase result;
        result.reserve(a_Dst.size());
        for (auto& level : a_Dst) {
            // remove levels that are not at least 4 in width/height
            if (level->GetSize().x >= 4 && level->GetSize().y >= 4)
                result.emplace_back(Compress2D(std::move(*level), a_Quality));
        }
        a_Dst = result;
    }
    a_Dst.SetCompressed(true);
    a_Dst.SetCompressionQuality(a_Quality);
    a_Dst.SetSize(a_Dst.front()->GetSize());
    a_Dst.SetPixelDescriptor(a_Dst.front()->GetPixelDescriptor());
}
