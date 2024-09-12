#include <SG/Core/Buffer/Buffer.hpp>
#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Image/Pixel.hpp>

#include <Tools/Debug.hpp>

#include <glm/common.hpp>
#include <glm/vec2.hpp>
namespace TabGraph::SG {
Image::Image()
    : Inherit()
{
    static size_t s_ImageNbr = 0;
    ++s_ImageNbr;
    SetName("Image_" + std::to_string(s_ImageNbr));
}

Image::Image(
    const Pixel::Description& a_PixelDesc,
    const size_t& a_Width, const size_t& a_Height, const size_t& a_Depth,
    const std::shared_ptr<BufferView>& a_BufferView)
    : Image()
{
    SetPixelDescription(a_PixelDesc);
    SetSize({ a_Width, a_Height, a_Depth });
    const auto pixelCount = a_Width * a_Height * a_Depth;
    BufferAccessor accessor(a_BufferView, 0, pixelCount, a_PixelDesc.GetDataType(), a_PixelDesc.GetComponents());
    SetBufferAccessor(accessor);
}

void Image::Allocate()
{
    const auto pixelCount = GetSize().x * GetSize().y * GetSize().z;
    BufferAccessor accessor(0, pixelCount, GetPixelDescription().GetDataType(), GetPixelDescription().GetComponents());
    SetBufferAccessor(accessor);
}

void Image::Blit(
    SG::Image& a_Dst,
    const glm::uvec3& a_Offset,
    const glm::uvec3& a_Size,
    const ImageFilter& a_Filter) const
{
    glm::uvec3 endPixel = a_Offset + a_Size;
    for (auto z = a_Offset.z; z < endPixel.z; z++) {
        auto UVz = z / float(endPixel.z);
        for (auto y = a_Offset.y; y < endPixel.y; y++) {
            auto UVy = y / float(endPixel.y);
            for (auto x = a_Offset.x; x < endPixel.x; x++) {
                auto UVx     = x / float(endPixel.x);
                glm::vec3 UV = { UVx, UVy, UVz };
                a_Dst.StoreNorm(UV, LoadNorm(UV, a_Filter));
            }
        }
    }
}

void Image::Fill(const Pixel::Color& a_Color)
{
    ApplyTreatment([a_Color](const auto&) { return a_Color; });
}

void Image::StoreNorm(const glm::vec3& a_UV, const Pixel::Color& a_Color)
{
    Store(glm::floor(a_UV * glm::vec3(GetSize())), a_Color);
}

Pixel::Color Image::Load(const Pixel::Coord& a_TexCoord) const
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferAccessor().empty() && "Image::SetColor : Unpacked Data is empty");
    return GetPixelDescription().GetColorFromBytes(_GetPointer(a_TexCoord));
}

void Image::Store(const Pixel::Coord& a_TexCoord, const Pixel::Color& a_Color)
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferAccessor().empty() && "Image::SetColor : Unpacked Data is empty");
    GetPixelDescription().SetColorToBytes(_GetPointer(a_TexCoord), a_Color);
}

std::byte* Image::_GetPointer(const Pixel::Coord& a_TexCoord)
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferAccessor().empty() && "Image::SetColor : Unpacked Data is empty");
    auto index = static_cast<size_t>((a_TexCoord.z * GetSize().x * GetSize().y) + (a_TexCoord.y * GetSize().x) + a_TexCoord.x);
    return &GetBufferAccessor().at(index);
}

std::byte* Image::_GetPointer(const Pixel::Coord& a_TexCoord) const
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferAccessor().empty() && "Image::SetColor : Unpacked Data is empty");
    auto index = static_cast<size_t>((a_TexCoord.z * GetSize().x * GetSize().y) + (a_TexCoord.y * GetSize().x) + a_TexCoord.x);
    return &GetBufferAccessor().at(index);
}
}
