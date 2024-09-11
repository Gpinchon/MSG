#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <Tools/Debug.hpp>
#include <Tools/ThreadPool.hpp>

#include <cmath>

#include <FasTC/CompressedImage.h>
#include <FasTC/CompressionFormat.h>
#include <FasTC/Image.h>
#include <FasTC/TexComp.h>

#include <glm/common.hpp>
#include <glm/vec2.hpp>

#define MIPMAPNBR2D(size) unsigned((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(std::max(size.x, size.y))))

namespace TabGraph::SG {
void GenerateCubemapMipMaps(Texture& a_Texture)
{
    Tools::ThreadPool threadPool;
    const auto pixelDesc      = a_Texture.GetPixelDescription();
    const glm::ivec2 baseSize = a_Texture.GetSize();
    const auto mipNbr         = MIPMAPNBR2D(baseSize);
    const auto baseLevel      = std::static_pointer_cast<SG::Cubemap>(a_Texture[0]);
    a_Texture.reserve(mipNbr);
    auto levelSrc = baseLevel;
    for (auto level = 1u; level <= mipNbr; level++) {
        auto levelSize = glm::max(baseSize / int(pow(2, level)), 1);
        auto mip       = std::make_shared<SG::Cubemap>(pixelDesc, levelSize.x, levelSize.y);
        mip->Allocate();
        a_Texture.emplace_back(mip);
        for (auto side = 0u; side < mip->GetSize().z; side++) {
            auto& sideSrc = levelSrc->at(side);
            auto& sideDst = mip->at(side);
            threadPool.PushCommand([&mip, &sideSrc, &sideDst] {
                for (auto y = 0u; y < mip->GetSize().y; y++) {
                    const auto yCoord = y / float(mip->GetSize().y);
                    for (auto x = 0u; x < mip->GetSize().x; x++) {
                        const auto xCoord = x / float(mip->GetSize().x);
                        const auto color  = sideSrc.LoadNorm({ xCoord, yCoord, 0 }, SG::ImageFilter::Bilinear);
                        sideDst.Store({ x, y, 0 }, color);
                    }
                }
            },
                false);
        }
        threadPool.Wait();
        levelSrc = mip;
    }
}

void Generate2DMipMaps(Texture& a_Texture)
{
    const auto pixelDesc      = a_Texture.GetPixelDescription();
    const glm::ivec2 baseSize = a_Texture.GetSize();
    const auto mipNbr         = MIPMAPNBR2D(baseSize) + 1;
    auto srcLevel             = std::static_pointer_cast<SG::Image2D>(a_Texture.front());
    a_Texture.resize(mipNbr);
    for (auto level = 1u; level < mipNbr; level++) {
        auto levelSize      = glm::max(baseSize / int(pow(2, level)), 1);
        auto mip            = std::make_shared<SG::Image2D>(pixelDesc, levelSize.x, levelSize.y);
        a_Texture.at(level) = mip;
        mip->Allocate();
        for (auto y = 0u; y < mip->GetSize().y; y++) {
            const auto yCoord = y / float(mip->GetSize().y);
            for (auto x = 0u; x < mip->GetSize().x; x++) {
                const auto xCoord = x / float(mip->GetSize().x);
                const auto color  = srcLevel->LoadNorm({ xCoord, yCoord, 0 }, ImageFilter::Bilinear);
                mip->Store({ x, y, 0 }, color);
            }
        }
        srcLevel = mip;
    }
}

void Texture::GenerateMipmaps()
{
    if (GetType() == TextureType::TextureCubemap)
        GenerateCubemapMipMaps(*this);
    else if (GetType() == TextureType::Texture2D)
        Generate2DMipMaps(*this);
}

auto Compress2D(SG::Image2D& a_Image, const uint8_t& a_Quality)
{
    auto inputSize                   = a_Image.GetSize();
    SG::BufferAccessor inputAccessor = a_Image.GetBufferAccessor();
    if (a_Image.GetPixelDescription().GetSizedFormat() != Pixel::SizedFormat::Uint8_NormalizedRGBA) {
        debugLog("Image is not Uint8_NormalizedRGBA, creating properly sized image");
        auto newImage = SG::Image2D(Pixel::SizedFormat::Uint8_NormalizedRGBA, inputSize.x, inputSize.y);
        newImage.Allocate();
        for (auto y = 0u; y < inputSize.y; y++)
            for (auto x = 0u; x < inputSize.x; x++)
                newImage.Store({ x, y, 0 }, a_Image.Load({ x, y, 0 }));
        inputAccessor = newImage.GetBufferAccessor();
    }
    SCompressionSettings settings;
    settings.format   = FasTC::eCompressionFormat_DXT5;
    settings.iQuality = a_Quality;
    FasTC::Image<FasTC::Pixel> image(inputSize.x, inputSize.y, (uint32_t*)&*inputAccessor.begin());
    std::unique_ptr<CompressedImage> compressedImage(CompressImage(&image, settings));
    auto outputSize    = glm::ivec2(compressedImage->GetWidth(), compressedImage->GetHeight());
    auto newBuffer     = std::make_shared<SG::Buffer>((std::byte*)compressedImage->GetCompressedData(), compressedImage->GetCompressedSize());
    auto newBufferView = std::make_shared<SG::BufferView>(newBuffer, 0, newBuffer->size());
    auto newImage      = std::make_shared<SG::Image2D>(Pixel::SizedFormat::DXT5_RGBA, inputSize.x, inputSize.y, newBufferView);
    return newImage;
}

void Texture::Compress(const uint8_t& a_Quality)
{
    if (GetPixelDescription().GetSizedFormat() == Pixel::SizedFormat::DXT5_RGBA) {
        debugLog("Texture already compressed");
        return;
    }
    if (GetType() == TextureType::Texture2D) {
        TextureBase result;
        result.reserve(size());
        for (auto& level : *this) {
            // remove levels that are not at least 4 in width/height
            if (level->GetSize().x >= 4 && level->GetSize().y >= 4)
                result.emplace_back(Compress2D(*std::static_pointer_cast<SG::Image2D>(level), a_Quality));
        }
        *this = result;
    }
    SetCompressed(true);
    SetCompressionQuality(a_Quality);
    SetSize(front()->GetSize());
    SetPixelDescription(front()->GetPixelDescription());
}
}
