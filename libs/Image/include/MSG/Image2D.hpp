#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Image.hpp>

#include <glm/vec2.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Image2D : public Core::Inherit<Image, Image2D> {
public:
    using Inherit::Inherit;
    Image2D(
        const PixelDescriptor& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height,
        const std::shared_ptr<BufferView>& a_BufferView = {})
        : Inherit(a_PixelDesc, a_Width, a_Height, 1, a_BufferView)
    {
    }
    ImageType GetType() const override
    {
        return ImageType::Image2D;
    }
    PixelColor LoadNorm(
        const glm::vec3& a_UV,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const override;
};
}
