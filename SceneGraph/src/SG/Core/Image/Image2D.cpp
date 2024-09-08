#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>
#include <Tools/Debug.hpp>

#include <glm/common.hpp>

#include <FasTC/CompressedImage.h>
#include <FasTC/CompressionFormat.h>
#include <FasTC/Image.h>
#include <FasTC/TexComp.h>

#include <cassert>

#define CLAMPX(texX) glm::clamp(float(texX), 0.f, float(GetSize().x - 1))
#define CLAMPY(texY) glm::clamp(float(texY), 0.f, float(GetSize().y - 1))

namespace TabGraph::SG {
Pixel::Color Image2D::LoadNorm(const glm::vec3& a_UV, const ImageFilter& a_Filter) const
{
    assert(!GetBufferAccessor().empty() && "Image2D::GetColor : Unpacked Data is empty");
    glm::vec2 uv0 = { CLAMPX(a_UV.x * GetSize().x), CLAMPY(a_UV.y * GetSize().y) };
    if (a_Filter == ImageFilter::Nearest) {
        return Load({ glm::round(uv0), 0 });
    }
    glm::vec2 uv1    = { CLAMPX(uv0.x + 1), CLAMPY(uv0.y + 1) };
    Pixel::Color c00 = Load({ uv0.x, uv0.y, 0 });
    Pixel::Color c10 = Load({ uv1.x, uv0.y, 0 });
    Pixel::Color c01 = Load({ uv0.x, uv1.y, 0 });
    Pixel::Color c11 = Load({ uv1.x, uv1.y, 0 });
    auto tx          = glm::fract(uv0.x);
    auto ty          = glm::fract(uv0.y);
    return Pixel::BilinearFilter(tx, ty, c00, c10, c01, c11);
}
std::shared_ptr<SG::Image> Image2D::Compress(const uint8_t& a_Quality) const
{
    if (GetPixelDescription().GetSizedFormat() == Pixel::SizedFormat::DXT5_RGBA) {
        debugLog("Image already compressed");
        return nullptr;
    }
    SG::BufferAccessor inputAccessor = GetBufferAccessor();
    if (GetPixelDescription().GetSizedFormat() != Pixel::SizedFormat::Uint8_NormalizedRGBA) {
        debugLog("Image is not Uint8_NormalizedRGBA, creating properly sized image");
        auto newImage = SG::Image2D(Pixel::SizedFormat::Uint8_NormalizedRGBA, GetSize().x, GetSize().y);
        newImage.Allocate();
        for (auto y = 0u; y < GetSize().y; y++) {
            for (auto x = 0u; x < GetSize().x; x++) {
                newImage.Store({ x, y, 0 }, Load({ x, y, 0 }));
            }
        }
        inputAccessor = newImage.GetBufferAccessor();
    }
    SCompressionSettings settings;
    settings.format   = FasTC::eCompressionFormat_DXT5;
    settings.iQuality = a_Quality;
    FasTC::Image<FasTC::Pixel> image(GetSize().x, GetSize().y, (uint32_t*)&*inputAccessor.begin());
    auto compressedImage = CompressImage(&image, settings);
    auto newBuffer       = std::make_shared<SG::Buffer>((std::byte*)compressedImage->GetCompressedData(), compressedImage->GetCompressedSize());
    auto newBufferView   = std::make_shared<SG::BufferView>(newBuffer, 0, newBuffer->size());
    auto newImage        = std::make_shared<SG::Image2D>(Pixel::SizedFormat::DXT5_RGBA, compressedImage->GetWidth(), compressedImage->GetHeight(), newBufferView);
    delete compressedImage;
    return newImage;
}
std::shared_ptr<SG::Image> Image2D::Decompress() const
{
    auto newImage = std::make_shared<SG::Image2D>(Pixel::SizedFormat::Uint8_NormalizedRGBA, GetSize().x, GetSize().y);
    newImage->Allocate();

    return newImage;
}
}
