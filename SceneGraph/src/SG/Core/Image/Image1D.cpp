#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image1D.hpp>

#include <glm/common.hpp>

#include <cassert>

#define CLAMPX(texX) glm::clamp(unsigned(texX), 0u, GetSize().x - 1)

namespace TabGraph::SG {
Pixel::Color Image1D::LoadNorm(const glm::vec3& a_UV, const ImageFilter& a_Filter) const
{
    assert(!GetBufferAccessor().empty() && "Image1D::GetColor : Unpacked Data is empty");
    glm::vec3 uv0 = a_UV * glm::vec3(GetSize());
    if (a_Filter == ImageFilter::Nearest)
        return Load(glm::round(uv0));
    auto tx          = glm::fract(uv0.x);
    Pixel::Color c00 = Load({ uv0.x, uv0.y, uv0.z });
    Pixel::Color c10 = Load({ CLAMPX(uv0.x + 1), uv0.y, uv0.z });
    return glm::mix(c00, c10, tx);
}
}
