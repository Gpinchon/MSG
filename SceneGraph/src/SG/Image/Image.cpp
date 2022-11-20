#include <SG/Image/Image.hpp>
#include <SG/Image/Pixel.hpp>
#include <SG/Buffer/View.hpp>

#include <glm/vec2.hpp>
#include <glm/common.hpp>

#define CLAMPX(texX) glm::clamp(int(texX), 0, GetSize().x - 1)
#define CLAMPY(texY) glm::clamp(int(texY), 0, GetSize().y - 1)
#define CLAMPZ(texZ) glm::clamp(int(texZ), 0, GetSize().z - 1)

namespace TabGraph::SG {
Pixel::Color Image::GetColor(const glm::vec3& uv0, Image::SamplingFilter filter)
{
    assert(!GetBufferView()->empty() && "Image::GetColor : Unpacked Data is empty");
    if (filter == Image::SamplingFilter::Nearest)
        return GetPixelDescription().GetColorFromBytes(_GetPointer(uv0));
    else if (GetType() == Type::Image1D) {
        auto tx = glm::fract(uv0.x);
        Pixel::Color c00 = GetColor({ uv0.x, uv0.y, uv0.z });
        Pixel::Color c10 = GetColor({ CLAMPX(uv0.x + 1), uv0.y, uv0.z });
        return glm::mix(c00, c10, tx);
    }
    else if (GetType() == Type::Image2D) {
        auto tx = glm::fract(uv0.x);
        auto ty = glm::fract(uv0.y);
        glm::vec2 uv1 = { CLAMPX(uv0.x + 1), CLAMPY(uv0.y + 1) };
        Pixel::Color c00 = GetColor({ uv0.x, uv0.y, uv0.z });
        Pixel::Color c10 = GetColor({ uv1.x, uv0.y, uv0.z });
        Pixel::Color c01 = GetColor({ uv0.x, uv1.y, uv0.z });
        Pixel::Color c11 = GetColor({ uv1.x, uv1.y, uv0.z });
        return Pixel::BilinearFilter(tx, ty, c00, c10, c01, c11);
    }
    else if (GetType() == Type::Image3D) {
        auto tx = glm::fract(uv0.x);
        auto ty = glm::fract(uv0.y);
        auto tz = glm::fract(uv0.z);
        glm::vec3 uv1 = { CLAMPX(uv0.x + 1), CLAMPY(uv0.y + 1), CLAMPZ(uv0.z + 1) };
        Pixel::Color c000 = GetColor({ uv0.x, uv0.y, uv0.z });
        Pixel::Color c100 = GetColor({ uv1.x, uv0.y, uv0.z });
        Pixel::Color c010 = GetColor({ uv0.x, uv1.y, uv0.z });
        Pixel::Color c110 = GetColor({ uv1.x, uv1.y, uv0.z });
        Pixel::Color c001 = GetColor({ uv0.x, uv0.y, uv1.z });
        Pixel::Color c101 = GetColor({ uv1.x, uv0.y, uv1.z });
        Pixel::Color c011 = GetColor({ uv0.x, uv1.y, uv1.z });
        Pixel::Color c111 = GetColor({ uv1.x, uv1.y, uv1.z });
        auto e = Pixel::BilinearFilter(tx, ty, c000, c100, c010, c110); 
        auto f = Pixel::BilinearFilter(tx, ty, c001, c101, c011, c111); 
        return glm::mix(e, f, tz);
    }
}

void Image::SetColor(const Pixel::Coord& texCoord, const glm::vec4& color)
{
    assert(!GetBufferView()->empty() && "Image::SetColor : Unpacked Data is empty");
    GetPixelDescription().SetColorToBytes(_GetPointer(texCoord), color);
}

std::byte* Image::_GetPointer(const Pixel::Coord& texCoord)
{
    auto index = GetPixelDescription().GetPixelIndex(GetSize(), texCoord);
    return &GetBufferView()->at(index);
}
}