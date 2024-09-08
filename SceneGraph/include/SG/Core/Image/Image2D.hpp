#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Inherit.hpp>

#include <glm/vec2.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Image2D : public Inherit<Image, Image2D> {
public:
    using Inherit::Inherit;
    Image2D(
        const Pixel::Description& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height,
        const std::shared_ptr<BufferView>& a_BufferView = {})
        : Inherit(a_PixelDesc, a_Width, a_Height, 1, a_BufferView)
    {
    }
    ImageType GetType() const override
    {
        return ImageType::Image2D;
    }
    Pixel::Color LoadNorm(
        const glm::vec3& a_UV,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const override;
    std::shared_ptr<SG::Image> Compress(const uint8_t& a_Quality) const override;
    std::shared_ptr<SG::Image> Decompress() const override;
};
}
