#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Image/Image2D.hpp>
#include <SG/Core/Image/Pixel.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

#include <array>
#include <glm/vec2.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
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
class Cubemap : public CubemapImageArray, public Inherit<Image, Cubemap> {
public:
    using Inherit::Inherit;
    Cubemap(
        const Pixel::Description& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height,
        const std::shared_ptr<BufferView>& a_BufferView = {});
    /**
     * @brief constructs a cubemap from an equirectangular image
     * @arg a_EquirectangularImage : the equirectangular image that will be converted to cubemap
     * @arg a_Size : the size of each images constituting the cubemap
     * @arg a_PixelDesc : the format of each images constituting the cubemap
     */
    Cubemap(
        const Pixel::Description& a_PixelDesc,
        const size_t& a_Width, const size_t& a_Height,
        const Image2D& a_EquirectangularImage);
    ~Cubemap() override = default;
    ImageType GetType() const override
    {
        return ImageType::Cubemap;
    }
    /// @brief Allocates the cubemap storage
    void Allocate() override;
    /// @brief Updates the sides of the cubemap with the current Buffer View
    void UpdateSides();
    Pixel::Color LoadNorm(
        const glm::vec3& a_Coords,
        const ImageFilter& a_Filter = ImageFilter::Nearest) const override;
    void StoreNorm(
        const glm::vec3& a_Coords,
        const Pixel::Color& a_Color) override;
    /// @brief converts the side/UV to normalized sampling dir
    /// @param a_Side the side to sample from
    /// @param a_UV the normalized UV coordinates
    /// @return the normalized sampling direction
    static glm::vec3 UVToXYZ(const CubemapSide& a_Side, const glm::vec2& a_UV);
};
}
