#include <MSG/Image.hpp>
#include <MSG/Image/ManhattanRound.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <glm/common.hpp>
#include <glm/mat3x3.hpp>
#include <glm/vec2.hpp>

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
    return std::move(GetStorage().Read(GetSize(), GetPixelDescriptor(), a_Offset, a_Size));
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

Image Image::GetLayer(const uint32_t& a_Layer) const
{
    return ImageInfo {
        .width     = GetSize().x,
        .height    = GetSize().y,
        .pixelDesc = GetPixelDescriptor(),
        .storage   = { GetStorage(), glm::uvec3(0, 0, a_Layer) }
    };
}

void Image::Blit(
    Image& a_Dst,
    const glm::uvec3& a_Offset,
    const glm::uvec3& a_Size) const
{
    Map();
    a_Dst.Map();
    glm::uvec3 endPixel = a_Offset + a_Size;
    for (auto z = a_Offset.z; z < endPixel.z; z++) {
        auto w = z / float(endPixel.z);
        for (auto y = a_Offset.y; y < endPixel.y; y++) {
            auto v = y / float(endPixel.y);
            for (auto x = a_Offset.x; x < endPixel.x; x++) {
                auto u     = x / float(endPixel.x);
                auto UVW   = glm::vec3(u, v, w);
                auto dstTc = UVW * glm::vec3(a_Dst.GetSize());
                a_Dst.Store(dstTc, Load({ x, y, z }));
            }
        }
    }
    a_Dst.Unmap();
    Unmap();
}

std::shared_ptr<Image> Image::Copy() const
{
    auto newImage = std::make_shared<Image>(*this);
    newImage->Allocate();
    Blit(*newImage, { 0, 0, 0 }, GetSize());
    return newImage;
}

void Image::Fill(const PixelColor& a_Color)
{
    std::vector<std::byte> pixels(GetSize().x * GetSize().y * GetSize().z * GetPixelDescriptor().GetPixelSize());
    GetPixelDescriptor().SetColorToBytesRange(
        std::to_address(pixels.begin()),
        std::to_address(pixels.end()),
        a_Color);
    GetStorage().Write(GetSize(), GetPixelDescriptor(), glm::uvec3(0u), GetSize(), std::move(pixels));
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

void Image::FlipX()
{
    Map();
    for (auto z = 0u; z < GetSize().z; z++) {
        for (auto y = 0u; y < GetSize().y; y++) {
            for (auto x = 0u; x < GetSize().x / 2; x++) {
                auto x1   = GetSize().x - (x + 1);
                auto temp = Load({ x, y, z });
                Store({ x, y, z }, Load({ x1, y, z }));
                Store({ x1, y, z }, temp);
            }
        }
    }
    Unmap();
}

void Image::FlipY()
{
    Map();
    for (auto z = 0u; z < GetSize().z; z++) {
        for (auto y = 0u; y < GetSize().y / 2; y++) {
            auto y1 = GetSize().y - (y + 1);
            for (auto x = 0u; x < GetSize().x; x++) {
                auto temp = Load({ x, y, z });
                Store({ x, y, z }, Load({ x, y1, z }));
                Store({ x, y1, z }, temp);
            }
        }
    }
    Unmap();
}

void Image::FlipZ()
{
    Map();
    for (auto z = 0u; z < GetSize().z / 2; z++) {
        auto z1 = GetSize().z - (z + 1);
        for (auto y = 0u; y < GetSize().y; y++) {
            for (auto x = 0u; x < GetSize().x; x++) {
                auto temp = Load({ x, y, z });
                Store({ x, y, z }, Load({ x, y, z1 }));
                Store({ x, y, z1 }, temp);
            }
        }
    }
    Unmap();
}

void Image::ApplyTransform(const glm::mat3x3& a_TexCoordTransform)
{
    Map();
    auto tempImg = Copy();
    for (auto z = 0u; z < GetSize().z; z++) {
        for (auto y = 0u; y < GetSize().y; y++) {
            for (auto x = 0u; x < GetSize().x; x++) {
                auto newCoord = a_TexCoordTransform * glm::vec3(x, y, z);
                newCoord      = glm::clamp(newCoord, { 0, 0, 0 }, glm::vec3(GetSize()) - 1.f);
                Store(newCoord, tempImg->Load({ x, y, z }));
            }
        }
    }
    Unmap();
}
}
