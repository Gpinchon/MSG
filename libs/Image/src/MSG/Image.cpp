#include <MSG/Image.hpp>
#include <MSG/PixelDescriptor.hpp>

namespace MSG {
Image::Image()
    : Inherit()
{
    static size_t s_ImageNbr = 0;
    ++s_ImageNbr;
    SetName("Image_" + std::to_string(s_ImageNbr));
}

Image::Image(const ImageInfo& a_Info)
    : Image()
{
    SetStorage(a_Info.storage);
    SetPixelDescriptor(a_Info.pixelDesc);
    SetSize({ a_Info.width, a_Info.height, a_Info.depth });
}

void MSG::Image::Write(const glm::uvec3& a_Offset, const glm::uvec3& a_Size, std::vector<std::byte>&& a_Data)
{
    return GetStorage().Write(GetSize(), GetPixelDescriptor(), a_Offset, a_Size, std::move(a_Data));
}

std::vector<std::byte> MSG::Image::Read(const glm::uvec3& a_Offset, const glm::uvec3& a_Size) const
{
    return GetStorage().Read(GetSize(), GetPixelDescriptor(), a_Offset, a_Size);
}

void MSG::Image::Map() const
{
    GetStorage().Map(GetSize(), GetPixelDescriptor(), glm::uvec3(0), GetSize());
}

void MSG::Image::Unmap() const
{
    GetStorage().Unmap(GetSize(), GetPixelDescriptor());
}

void Image::Allocate()
{
    GetStorage().Allocate(GetSize(), GetPixelDescriptor());
}

PixelColor Image::Load(const PixelCoord& a_TexCoord) const
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    return GetStorage().Read(GetSize(), GetPixelDescriptor(), a_TexCoord);
}

void Image::Store(const PixelCoord& a_TexCoord, const PixelColor& a_Color)
{
    assert(a_TexCoord.x < GetSize().x && a_TexCoord.y < GetSize().y && a_TexCoord.z < GetSize().z);
    GetStorage().Write(GetSize(), GetPixelDescriptor(), a_TexCoord, a_Color);
}
}
