#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image3D.hpp>

#include <glm/common.hpp>

#include <cassert>

#define CLAMPX(texX) glm::clamp(unsigned(texX), 0u, GetSize().x - 1)
#define CLAMPY(texY) glm::clamp(unsigned(texY), 0u, GetSize().y - 1)
#define CLAMPZ(texZ) glm::clamp(unsigned(texZ), 0u, GetSize().z - 1)

namespace TabGraph::SG {
Pixel::Color Image3D::LoadNorm(const glm::vec3& a_UV, const ImageFilter& a_Filter) const
{
    assert(!GetBufferAccessor().empty() && "Image3D::GetColor : Unpacked Data is empty");
    glm::vec3 uv0 = a_UV * glm::vec3(GetSize());
    if (a_Filter == ImageFilter::Nearest)
        return Load(glm::round(uv0));
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
std::shared_ptr<SG::Image> Image3D::Compress(const uint8_t& a_Quality) const
{
    return std::shared_ptr<SG::Image>();
}
std::shared_ptr<SG::Image> Image3D::Decompress() const
{
    return std::shared_ptr<SG::Image>();
}
}
