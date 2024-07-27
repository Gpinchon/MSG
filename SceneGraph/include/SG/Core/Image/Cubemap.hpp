#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Image/Image.hpp>
#include <SG/Core/Image/Pixel.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Property.hpp>

#include <array>
#include <glm/vec2.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
enum class ImageFilter;
class Image;
}

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
    NegativeZ
};
using CubemapImageArray = std::array<std::shared_ptr<Image>, 6>;
class Cubemap : private CubemapImageArray, public Inherit<Image, Cubemap> {
public:
    using Inherit::Inherit;
    /**
     * @brief constructs a cubemap from an equirectangular image
     * @arg a_EquirectangularImage : the equirectangular image that will be converted to cubemap
     * @arg a_Size : the size of each images constituting the cubemap
     * @arg a_PixelDesc : the format of each images constituting the cubemap
     */
    Cubemap(
        const std::shared_ptr<Image>& a_EquirectangularImage,
        const Pixel::Description& a_PixelDesc,
        const glm::ivec2& a_Size);
    ~Cubemap() override = default;
    Pixel::Color Load(
        const Pixel::Coord& a_Coords) const override;
    void Store(
        const Pixel::Coord& a_Coords,
        const Pixel::Color& a_Color) override;
    Pixel::Color LoadNorm(
        const glm::vec3& a_Coords,
        const ImageFilter& a_Filter) const override;
    void StoreNorm(
        const glm::vec3& a_Coords,
        const Pixel::Color& a_Color) override;
};
}