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
    SetName("Image_" + std::to_string(++s_ImageNbr));
}

Image::Image(
    const ImageType& a_Type,
    const Pixel::Description& a_PixelDesc,
    const glm::uvec3& a_Size,
    const std::shared_ptr<BufferView>& a_BufferView)
    : Image()
{
    SetType(a_Type);
    SetPixelDescription(a_PixelDesc);
    SetSize(a_Size);
    SetBufferView(a_BufferView);
}

Pixel::Color Image::LoadNorm(const glm::vec3& a_UV, const ImageFilter& a_Filter) const
{
    debugLog("Untyped images cannot use filtered samples, please use typed constructors");
    return Pixel::Color(0);
}

void Image::StoreNorm(const glm::vec3& a_UV, const Pixel::Color& a_Color)
{
    Store(glm::round(a_UV * glm::vec3(GetSize())), a_Color);
}

Pixel::Color Image::Load(const glm::uvec3& a_TexCoord) const
{
    return GetPixelDescription().GetColorFromBytes(_GetPointer(a_TexCoord));
}

void Image::Store(const Pixel::Coord& a_TexCoord, const glm::vec4& a_Color)
{
    assert(!GetBufferView()->empty() && "Image::SetColor : Unpacked Data is empty");
    GetPixelDescription().SetColorToBytes(_GetPointer(a_TexCoord), a_Color);
}

std::byte* Image::_GetPointer(const Pixel::Coord& a_TexCoord)
{
    auto index = GetPixelDescription().GetPixelIndex(GetSize(), a_TexCoord);
    return &GetBufferView()->at(index);
}

std::byte* Image::_GetPointer(const Pixel::Coord& a_TexCoord) const
{
    auto index = GetPixelDescription().GetPixelIndex(GetSize(), a_TexCoord);
    return &GetBufferView()->at(index);
}
}
