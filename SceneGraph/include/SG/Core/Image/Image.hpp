#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Image/Pixel.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class BufferView;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
enum class ImageType {
    Unknown,
    Image1D,
    Image2D,
    Image3D,
    ImageCubemap,
    MaxValue
};
enum class ImageFilter {
    Nearest,
    Bilinear,
    MaxValue
};
class Image : public Inherit<Object, Image> {
public:
    PROPERTY(ImageType, Type, ImageType::Unknown);
    PROPERTY(Pixel::Description, PixelDescription, );
    PROPERTY(glm::ivec3, Size, 0);
    PROPERTY(std::shared_ptr<BufferView>, BufferView, );

protected:
    Image(
        const ImageType& a_Type,
        const Pixel::Description& a_PixelDesc,
        const glm::uvec3& a_Size,
        const std::shared_ptr<BufferView>& a_BufferView = {});

public:
    Image();
    virtual ~Image() = default;

    /**
     * @brief Samples a color from the UV coordinates, asserts that _data is not empty
     * @param a_UV : the normalized texture coordinate to fetch the color from
     * @param a_Filter : the filtering to be used for sampling, default is nearest
     * @return the unpacked color
     */
    virtual Pixel::Color LoadNorm(
        const glm::vec3& a_UV,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const;
    /**
     * @brief Samples the color from the UV coordinates
     * @param a_UV : the normalized texture coordinates
     */
    virtual void StoreNorm(
        const glm::vec3& a_UV,
        const Pixel::Color& a_Color);
    /**
     * @brief Fetches a color from the specified pixel, asserts that _data is not empty
     * @param a_TexCoord : the pixel coordinate
     */
    virtual Pixel::Color Load(
        const Pixel::Coord& a_TexCoord) const;
    /**
     * @brief Sets the pixel corresponding to texCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color : the new color of this pixel
     */
    virtual void Store(
        const Pixel::Coord& a_TexCoord,
        const Pixel::Color& a_Color);

private:
    std::byte* _GetPointer(const Pixel::Coord& a_TexCoord);
    std::byte* _GetPointer(const Pixel::Coord& a_TexCoord) const;
};
}
