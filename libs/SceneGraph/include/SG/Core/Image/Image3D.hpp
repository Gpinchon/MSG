#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Inherit.hpp>

#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG {
class Image3D : public Inherit<Image, Image3D> {
public:
    using Inherit::Inherit;
    Image3D(
        const Pixel::Description& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height, const size_t& a_Depth,
        const std::shared_ptr<BufferView>& a_BufferView = {})
        : Inherit(a_PixelDesc, a_Width, a_Height, a_Depth, a_BufferView)
    {
    }
    ImageType GetType() const override
    {
        return ImageType::Image3D;
    }
    Pixel::Color LoadNorm(
        const glm::vec3& a_UV,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const override;
};
}
