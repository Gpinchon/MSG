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
    SetBufferView(a_BufferView);
}

void Image::Allocate()
{
    const auto textureByteSize = GetPixelDescription().GetSize() * GetSize().x * GetSize().y * GetSize().z;
    SetBufferView(std::make_shared<BufferView>(std::make_shared<Buffer>(textureByteSize), 0, textureByteSize));
}

Pixel::Color Image::LoadNorm(const glm::vec3& a_UV, const ImageFilter& a_Filter) const
{
    debugLog("Untyped images cannot use filtered samples, please use typed constructors");
    return Pixel::Color(0);
}

void Image::Fill(const Pixel::Color& a_Color)
{
    ApplyTreatment([a_Color](const auto&) { return a_Color; });
}

void Image::StoreNorm(const glm::vec3& a_UV, const Pixel::Color& a_Color)
{
    Store(glm::round(a_UV * glm::vec3(GetSize())), a_Color);
}

Pixel::Color Image::Load(const glm::uvec3& a_TexCoord) const
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferView()->empty() && "Image::SetColor : Unpacked Data is empty");
    return GetPixelDescription().GetColorFromBytes(_GetPointer(a_TexCoord));
}

void Image::Store(const Pixel::Coord& a_TexCoord, const glm::vec4& a_Color)
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferView()->empty() && "Image::SetColor : Unpacked Data is empty");
    GetPixelDescription().SetColorToBytes(_GetPointer(a_TexCoord), a_Color);
}

std::byte* Image::_GetPointer(const Pixel::Coord& a_TexCoord)
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferView()->empty() && "Image::SetColor : Unpacked Data is empty");
    auto index = GetPixelDescription().GetPixelIndex(GetSize(), a_TexCoord);
    return &GetBufferView()->at(index);
}

std::byte* Image::_GetPointer(const Pixel::Coord& a_TexCoord) const
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    assert(!GetBufferView()->empty() && "Image::SetColor : Unpacked Data is empty");
    auto index = GetPixelDescription().GetPixelIndex(GetSize(), a_TexCoord);
    return &GetBufferView()->at(index);
}
}
