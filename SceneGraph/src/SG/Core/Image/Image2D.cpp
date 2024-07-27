#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image2D.hpp>

#include <glm/common.hpp>

#include <cassert>

#define CLAMPX(texX) glm::clamp(int(texX), 0, GetSize().x - 1)
#define CLAMPY(texY) glm::clamp(int(texY), 0, GetSize().y - 1)

namespace TabGraph::SG {
Pixel::Color Image2D::LoadNorm(const glm::vec3& a_UV, const ImageFilter& a_Filter) const
{
    assert(!GetBufferView()->empty() && "Image2D::GetColor : Unpacked Data is empty");
    glm::vec3 uv0 = a_UV * glm::vec3(GetSize());
    if (a_Filter == ImageFilter::Nearest)
        return Load(glm::round(uv0));
    auto tx          = glm::fract(uv0.x);
    auto ty          = glm::fract(uv0.y);
    glm::vec2 uv1    = { CLAMPX(uv0.x + 1), CLAMPY(uv0.y + 1) };
    Pixel::Color c00 = Load({ uv0.x, uv0.y, uv0.z });
    Pixel::Color c10 = Load({ uv1.x, uv0.y, uv0.z });
    Pixel::Color c01 = Load({ uv0.x, uv1.y, uv0.z });
    Pixel::Color c11 = Load({ uv1.x, uv1.y, uv0.z });
    return Pixel::BilinearFilter(tx, ty, c00, c10, c01, c11);
}
}
