#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>

#include <glm/common.hpp>

#include <cassert>

#define CLAMPX(texX) glm::clamp(float(texX), 0.f, float(GetSize().x - 1))
#define CLAMPY(texY) glm::clamp(float(texY), 0.f, float(GetSize().y - 1))

namespace TabGraph::SG {
Pixel::Color Image2D::LoadNorm(const glm::vec3& a_UV, const ImageFilter& a_Filter) const
{
    assert(!GetBufferView()->empty() && "Image2D::GetColor : Unpacked Data is empty");
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
}
