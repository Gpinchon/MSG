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

public:
    Image();
    Image(
        const ImageType& a_Type,
        const Pixel::Description& a_PixelDesc,
        const glm::ivec3 a_Size,
        const std::shared_ptr<BufferView>& a_BufferView = {});
    virtual ~Image() = default;
    /**
     * @brief Fetches a color from the coordinates, asserts thad _data is not empty
     * @param a_TexCoord : the texture coordinate to fetch the color from
     * @param a_Filter : the filtering to be used for sampling, default is nearest
     * @return the unpacked color
     */
    virtual Pixel::Color GetColor(
        const glm::vec3& a_TexCoord,
        ImageFilter a_Filter = ImageFilter::Nearest);
    /**
     * @brief Sets the pixel corresponding to texCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color : the new color of this pixel
     */
    virtual void SetColor(const Pixel::Coord& texCoord, const Pixel::Color& color);

private:
    std::byte* _GetPointer(const Pixel::Coord& texCoord);
};
}
