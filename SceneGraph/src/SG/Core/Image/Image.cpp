#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Image/Pixel.hpp>

#include <glm/common.hpp>
#include <glm/vec2.hpp>

#define CLAMPX(texX) glm::clamp(int(texX), 0, GetSize().x - 1)
#define CLAMPY(texY) glm::clamp(int(texY), 0, GetSize().y - 1)
#define CLAMPZ(texZ) glm::clamp(int(texZ), 0, GetSize().z - 1)

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
    const glm::ivec3 a_Size,
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
    assert(!GetBufferView()->empty() && "Image::GetColor : Unpacked Data is empty");
    glm::vec3 uv0 = a_UV * glm::vec3(GetSize());
    if (a_Filter == ImageFilter::Nearest)
        return Image::Load(glm::round(uv0));
    else if (GetType() == ImageType::Image1D) {
        auto tx          = glm::fract(uv0.x);
        Pixel::Color c00 = Load({ uv0.x, uv0.y, uv0.z });
        Pixel::Color c10 = Load({ CLAMPX(uv0.x + 1), uv0.y, uv0.z });
        return glm::mix(c00, c10, tx);
    } else if (GetType() == ImageType::Image2D) {
        auto tx          = glm::fract(uv0.x);
        auto ty          = glm::fract(uv0.y);
        glm::vec2 uv1    = { CLAMPX(uv0.x + 1), CLAMPY(uv0.y + 1) };
        Pixel::Color c00 = Load({ uv0.x, uv0.y, uv0.z });
        Pixel::Color c10 = Load({ uv1.x, uv0.y, uv0.z });
        Pixel::Color c01 = Load({ uv0.x, uv1.y, uv0.z });
        Pixel::Color c11 = Load({ uv1.x, uv1.y, uv0.z });
        return Pixel::BilinearFilter(tx, ty, c00, c10, c01, c11);
    } else if (GetType() == ImageType::Image3D) {
        auto tx           = glm::fract(uv0.x);
        auto ty           = glm::fract(uv0.y);
        auto tz           = glm::fract(uv0.z);
        glm::vec3 uv1     = { CLAMPX(uv0.x + 1), CLAMPY(uv0.y + 1), CLAMPZ(uv0.z + 1) };
        Pixel::Color c000 = Load({ uv0.x, uv0.y, uv0.z });
        Pixel::Color c100 = Load({ uv1.x, uv0.y, uv0.z });
        Pixel::Color c010 = Load({ uv0.x, uv1.y, uv0.z });
        Pixel::Color c110 = Load({ uv1.x, uv1.y, uv0.z });
        Pixel::Color c001 = Load({ uv0.x, uv0.y, uv1.z });
        Pixel::Color c101 = Load({ uv1.x, uv0.y, uv1.z });
        Pixel::Color c011 = Load({ uv0.x, uv1.y, uv1.z });
        Pixel::Color c111 = Load({ uv1.x, uv1.y, uv1.z });
        auto e            = Pixel::BilinearFilter(tx, ty, c000, c100, c010, c110);
        auto f            = Pixel::BilinearFilter(tx, ty, c001, c101, c011, c111);
        return glm::mix(e, f, tz);
    }
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
