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
class Image : public Inherit<Object, Image> {
public:
    PROPERTY(Pixel::Description, PixelDescription, );
    PROPERTY(glm::uvec3, Size, 0);
    PROPERTY(std::shared_ptr<BufferView>, BufferView, );

protected:
    Image();
    Image(
        const Pixel::Description& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height, const size_t& a_Depth,
        const std::shared_ptr<BufferView>& a_BufferView = {});

public:
    virtual ~Image() = default;
    virtual void Allocate();
    virtual ImageType GetType() const
    {
        return ImageType::Unknown;
    }

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
     * @brief Fills the image with specified color
     */
    virtual void Fill(
        const Pixel::Color& a_Color);
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
    /// @brief Applies a function on each pixel
    template <typename Op>
    void ApplyTreatment(const Op& a_Op);

private:
    std::byte* _GetPointer(const Pixel::Coord& a_TexCoord);
    std::byte* _GetPointer(const Pixel::Coord& a_TexCoord) const;
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
