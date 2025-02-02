#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/Image2D.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <array>
#include <glm/vec2.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
enum class CubemapSide {
    PositiveX,
    NegativeX,
    PositiveY,
    NegativeY,
    PositiveZ,
    NegativeZ,
    MaxValue
};
using CubemapImageArray = std::array<Image2D, 6>;
class Cubemap : public CubemapImageArray, public Core::Inherit<Image, Cubemap> {
public:
    using Inherit::Inherit;
    Cubemap(
        const PixelDescriptor& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height,
        const std::shared_ptr<BufferView>& a_BufferView = {});
    /**
     * @brief constructs a cubemap from an equirectangular image
     * @arg a_EquirectangularImage : the equirectangular image that will be converted to cubemap
     * @arg a_Size : the size of each images constituting the cubemap
     * @arg a_PixelDesc : the format of each images constituting the cubemap
     */
    Cubemap(
        const PixelDescriptor& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height,
        const Image2D& a_EquirectangularImage);
    ~Cubemap() override = default;
    ImageType GetType() const override { return ImageType::Cubemap; }
    /**
     * @brief Updates the sides of the cubemap with the current Buffer View
     */
    void UpdateSides();
    /**
     * @brief converts the side/UV to normalized sampling dir
     * @param a_Side the side to sample from
     * @param a_UV the normalized UV coordinates
     * @return the normalized sampling direction
     */
    static glm::vec3 UVToXYZ(
        const CubemapSide& a_Side,
        const glm::vec2& a_UV);

    void Allocate() override;
    PixelColor LoadNorm(
        const glm::vec3& a_Coords,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const override;
    void StoreNorm(
        const glm::vec3& a_Coords,
        const PixelColor& a_Color) override;
};
}
