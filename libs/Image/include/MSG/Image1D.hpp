#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Image.hpp>

#include <stddef.h>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Image1D : public Core::Inherit<Image, Image1D> {
public:
    using Inherit::Inherit;
    Image1D(
        const PixelDescriptor& a_PixelDesc,
        const size_t& a_Width,
        const std::shared_ptr<BufferView>& a_BufferView = {})
        : Inherit(a_PixelDesc, a_Width, 1, 1, a_BufferView)
    {
    }
    ImageType GetType() const override
    {
        return ImageType::Image1D;
    }
    PixelColor LoadNorm(
        const glm::vec3& a_UV,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const override;
};
}
