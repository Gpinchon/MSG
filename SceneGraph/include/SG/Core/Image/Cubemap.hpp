#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Image/Pixel.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <array>
#include <glm/vec2.hpp>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
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
class Cubemap : public CubemapImageArray, public Inherit<Object, Cubemap> {
public:
    using CubemapImageArray::CubemapImageArray;
    using Inherit::Inherit;
    PROPERTY(Pixel::Description, PixelDesc, {});
    PROPERTY(glm::ivec2, Size, {});
    /**
     * @brief constructs a cubemap from an equirectangular image
     * @arg a_EquirectangularImage : the equirectangular image that will be converted to cubemap
     * @arg a_Size : the size of each images constituting the cubemap
     * @arg a_PixelDesc : the format of each images constituting the cubemap
     */
    Cubemap(
        const std::shared_ptr<Image>& a_EquirectangularImage,
        const glm::ivec2& a_Size,
        const Pixel::Description& a_PixelDesc);
    virtual ~Cubemap() = default;
    Pixel::Color GetColor(
        const glm::vec3& a_Coords,
        const ImageFilter& a_Filter);
    void SetColor(
        const glm::vec3& a_Coords,
        const Pixel::Color& a_Color);
};
}