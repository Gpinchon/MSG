#include <MSG/Image/Cubemap.hpp>
#include <MSG/Sampler.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/Debug.hpp>
#include <MSG/Tools/ThreadPool.hpp>

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
auto CreateMip(const MSG::PixelDescriptor& a_PD, const glm::ivec3& a_BaseSize, const glm::ivec3& a_LevelSize)
{
    glm::ivec3 size = a_BaseSize;
    for (uint8_t i = 0; i < Dimension; ++i)
        size[i] = a_LevelSize[i];
    return std::make_shared<Image>(a_PD, size.x, size.y, size.z);
}

template <typename SamplerType>
void LvlGenFunc(Tools::ThreadPool& a_Tp, const SamplerType& a_Sampler, const uint32_t& a_LvlIndex, const Image& a_Src, Image& a_Dst)
{
    for (uint32_t z = 0; z < a_Dst.GetSize().z; z++) {
        float w = z / float(a_Dst.GetSize().z);
        for (uint32_t y = 0; y < a_Dst.GetSize().y; y++) {
            float v = y / float(a_Dst.GetSize().y);
            for (uint32_t x = 0; x < a_Dst.GetSize().x; x++) {
                float u = x / float(a_Dst.GetSize().x);
                a_Dst.Store({ x, y, z }, a_Sampler.Sample(a_Src, glm::vec3(u, v, w)));
            }
        }
    }
}

template <>
void LvlGenFunc(Tools::ThreadPool& a_Tp, const SamplerCube& a_Sampler, const uint32_t& a_LvlIndex, const Image& a_Src, Image& a_Dst)
{
    auto tcMax = glm::vec2(a_Dst.GetSize() - 1u);
    for (auto side = 0u; side < 6; side++) {
        a_Tp.PushCommand([a_Sampler, a_Src, a_Dst, tcMax, side]() mutable {
            for (uint32_t y = 0u; y < a_Dst.GetSize().y; y++) {
                auto v = y / tcMax.y;
                for (uint32_t x = 0u; x < a_Dst.GetSize().x; x++) {
                    auto u   = x / tcMax.x;
                    auto dir = CubemapUVWToSampleDir({ u, v, side });
                    a_Dst.Store({ x, y, side }, a_Sampler.Sample(a_Src, dir));
                }
            }
        },
            false);
    }
}

template <uint8_t Dimension, typename SamplerType>
void GenerateMipMaps(Texture& a_Texture, const SamplerType& a_Sampler = {})
{
    Tools::ThreadPool threadPool;
    const auto pixelDesc = a_Texture.GetPixelDescriptor();
    const auto baseSize  = glm::ivec3(a_Texture.GetSize());
    const auto mipNbr    = GetMipCount(baseSize) + 1;
    auto srcLevel        = a_Texture.front();
    a_Texture.resize(mipNbr);
    for (auto level = 1u; level < mipNbr; level++) {
        auto levelSize      = glm::max(baseSize / int(pow(2, level)), 1);
        auto mip            = CreateMip<Dimension>(pixelDesc, baseSize, levelSize);
        a_Texture.at(level) = mip;
        mip->Allocate();
        LvlGenFunc(threadPool, a_Sampler, level, *srcLevel, *mip);
        threadPool.Wait();
        srcLevel = mip;
    }
}

void Texture::GenerateMipmaps()
{
    if (GetType() == TextureType::Texture1D)
        GenerateMipMaps<1, Sampler1D>(*this);
    else if (GetType() == TextureType::Texture2D)
        GenerateMipMaps<2, Sampler2D>(*this);
    else if (GetType() == TextureType::Texture3D)
        GenerateMipMaps<3, Sampler3D>(*this);
    else if (GetType() == TextureType::TextureCubemap)
        GenerateMipMaps<2, SamplerCube>(*this);
    else
        errorLog("Mipmap generation not implemented for this texture type yet");
}

auto Compress2D(Image& a_Image, const uint8_t& a_Quality)
{
    auto inputSize               = a_Image.GetSize();
    BufferAccessor inputAccessor = a_Image.GetBufferAccessor();
    if (a_Image.GetPixelDescriptor().GetSizedFormat() != PixelSizedFormat::Uint8_NormalizedRGBA) {
        debugLog("Image is not Uint8_NormalizedRGBA, creating properly sized image");
        auto newImage = Image(PixelSizedFormat::Uint8_NormalizedRGBA, inputSize.x, inputSize.y, 1);
        newImage.Allocate();
        a_Image.Blit(newImage, { 0u, 0u, 0u }, a_Image.GetSize());
        inputAccessor = newImage.GetBufferAccessor();
    }
    SCompressionSettings settings;
    settings.format   = FasTC::eCompressionFormat_DXT5;
    settings.iQuality = a_Quality;
    FasTC::Image<FasTC::Pixel> image(inputSize.x, inputSize.y, reinterpret_cast<const uint32_t*>(std::to_address(inputAccessor.begin())));
    std::unique_ptr<CompressedImage> compressedImage(CompressImage(&image, settings));
    auto outputSize    = glm::ivec2(compressedImage->GetWidth(), compressedImage->GetHeight());
    auto newBuffer     = std::make_shared<Buffer>(reinterpret_cast<const std::byte*>(compressedImage->GetCompressedData()), compressedImage->GetCompressedSize());
    auto newBufferView = std::make_shared<BufferView>(newBuffer, 0, newBuffer->size());
    auto newImage      = std::make_shared<Image>(PixelSizedFormat::DXT5_RGBA, inputSize.x, inputSize.y, 1, newBufferView);
    return newImage;
}

void Texture::Compress(const uint8_t& a_Quality)
{
    if (GetPixelDescriptor().GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        debugLog("Texture already compressed");
        return;
    }
    if (GetType() == TextureType::Texture2D) {
        TextureBase result;
        result.reserve(size());
        for (auto& level : *this) {
            // remove levels that are not at least 4 in width/height
            if (level->GetSize().x >= 4 && level->GetSize().y >= 4)
                result.emplace_back(Compress2D(*level, a_Quality));
        }
        *this = result;
    }
    SetCompressed(true);
    SetCompressionQuality(a_Quality);
    SetSize(front()->GetSize());
    SetPixelDescriptor(front()->GetPixelDescriptor());
}
}
