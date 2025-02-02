#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/PixelDescriptor.hpp>

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

public:
    virtual ~Image() = default;
    virtual void Allocate();
    virtual ImageType GetType() const = 0;

    /**
     * @brief Samples a color from the UV coordinates, asserts that _data is not empty
     * @param a_UV : the normalized texture coordinate to fetch the color from
     * @param a_Filter : the filtering to be used for sampling, default is nearest
     * @return the unpacked color
     */
    virtual PixelColor LoadNorm(
        const glm::vec3& a_UV,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const
        = 0;
    /**
     * @brief blits the image to the destination
     */
    void Blit(
        Image& a_Dst,
        const glm::uvec3& a_Offset,
        const glm::uvec3& a_Size,
        const ImageFilter& a_Filter) const;
    /**
     * @brief Fills the image with specified color
     */
    virtual void Fill(
        const PixelColor& a_Color);
    /**
     * @brief Samples the color from the UV coordinates
     * @param a_UV : the normalized texture coordinates
     */
    virtual void StoreNorm(
        const glm::vec3& a_UV,
        const PixelColor& a_Color);
    /**
     * @brief Fetches a color from the specified pixel, asserts that _data is not empty
     * @param a_TexCoord : the pixel coordinate
     */
    virtual PixelColor Load(
        const PixelCoord& a_TexCoord) const;
    /**
     * @brief Sets the pixel corresponding to texCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color : the new color of this pixel
     */
    virtual void Store(
        const PixelCoord& a_TexCoord,
        const PixelColor& a_Color);
    /// @brief Applies a function on each pixel
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
