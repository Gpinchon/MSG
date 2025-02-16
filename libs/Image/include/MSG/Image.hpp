#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <glm/fwd.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
enum class ImageType {
    Unknown,
    Cubemap,
    Image1D,
    Image2D,
    Image3D,
    MaxValue
};
enum class ImageFilter {
    Nearest,
    Bilinear,
    MaxValue
};

class Image : public Core::Inherit<Core::Object, Image> {
public:
    PROPERTY(PixelDescriptor, PixelDescriptor, );
    PROPERTY(glm::uvec3, Size, 0);
    PROPERTY(BufferAccessor, BufferAccessor, );

protected:
    Image();
    Image(
        const PixelDescriptor& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height, const size_t& a_Depth,
        const std::shared_ptr<BufferView>& a_BufferView = {});
    Image(const Image&) = default;

public:
    virtual ~Image() = default;
    virtual void Allocate();
    virtual ImageType GetType() const = 0;
    /**
     * @brief Creates an exact copy of this image
     * @attention The newly created image WON'T share pixel buffer
     *
     */
    std::shared_ptr<Image> Copy() const;
    /**
     * @brief Creates a clone of this image
     * @attention The newly created image WILL share pixel buffer
     *
     */
    virtual std::shared_ptr<Image> Clone() const = 0;
    /** @brief Fills the image with specified color */
    virtual void Fill(const PixelColor& a_Color);
    /** @brief Blits the image to the destination */
    void Blit(
        Image& a_Dst,
        const glm::uvec3& a_Offset,
        const glm::uvec3& a_Size) const;
    /** @brief Fetches a color from the specified pixel, asserts that _data is not empty */
    PixelColor Load(const PixelCoord& a_TexCoord) const;
    /**
     * @brief Sets the pixel corresponding to texCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color : the new color of this pixel
     */
    void Store(
        const PixelCoord& a_TexCoord,
        const PixelColor& a_Color);
    /** @brief Flips the image on the specified axis */
    void FlipX();
    /** @copydoc MSG::Image::FlipX */
    void FlipY();
    /** @copydoc MSG::Image::FlipX */
    void FlipZ();
    /**
     * @brief Applies the specified transform to each pixel.
     * @attention This operation will create a temporary image buffer
     *
     * @param a_TexCoordTransform a matrix to transform the pixel coordinates
     */
    void ApplyTransform(const glm::mat3x3& a_TexCoordTransform);
    /**
     * @brief Applies a function on each pixel
     *
     * @tparam Op type of a_Op
     * @param a_Op the function to apply to each pixel
     */
    template <typename Op>
    void ApplyTreatment(const Op& a_Op);

private:
    std::byte* _GetPointer(const PixelCoord& a_TexCoord);
    std::byte* _GetPointer(const PixelCoord& a_TexCoord) const;
};

template <typename Op>
void Image::ApplyTreatment(const Op& a_Op)
{
    for (auto z = 0u; z < GetSize().z; ++z) {
        for (auto y = 0u; y < GetSize().y; ++y) {
            for (auto x = 0u; x < GetSize().x; ++x) {
                Store({ x, y, z }, a_Op(Load({ x, y, z })));
            }
        }
    }
}
}
