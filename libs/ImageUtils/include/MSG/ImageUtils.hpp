#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Image.hpp>

#include <glm/fwd.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
/**
 * @brief constructs a cubemap from an equirectangular image
 * @param a_PixelDesc the format of each images constituting the cubemap
 * @param a_Width the width of each images constituting the cubemap
 * @param a_Height the height of each images constituting the cubemap
 * @param a_EquirectangularImage the equirectangular image that will be converted to cubemap
 * @return a 2D images array representing the newly created cubemap
 */
Image CubemapFromEqui(
    const PixelDescriptor& a_PixelDesc,
    const uint32_t& a_Width, const uint32_t& a_Height,
    Image& a_EquirectangularImage);
/**
 * @brief Constructs a cubemap image from the sides
 * @attention every image in array must be the same size
 * @param a_Sides the sides to construct from in the order specified by CubemapSide
 * @return a 2D images array representing the newly created cubemap
 */
Image CubemapFromSides(const std::array<Image, 6>& a_Sides);
/**
 * @brief Converts a normalized cubemap sampling direction
 * to equirectangular normalized coordinates
 * @param a_SampleVec the normalized cubemap sampling direction
 * @return normalized 2D sampling coordinates
 */
glm::vec2 CubemapSampleVecToEqui(glm::vec3 a_SampleVec);

Image ImageCompress(const Image& a_Src);
Image ImageDecompress(const Image& a_Src);
std::vector<std::byte> ImageDecompress(const Image& a_Src, const glm::uvec3& a_Offset, const glm::uvec3& a_Size);

/** @brief creates a new image from src and returns it */
Image ImageResize(const Image& a_Src, const glm::uvec3& a_NewSize);
/**
 * @brief Creates an exact copy of the provided image
 * @attention The newly created image WON'T share pixel buffer
 */
Image ImageCopy(const Image& a_Src);
/**
 * @brief Constructs an image from the specified layer
 * @attention The newly created image WILL share pixel buffer
 */
Image ImageGetLayer(const Image& a_Src, const uint32_t& a_Layer);
/** @brief Fills the image with specified color */
void ImageFill(Image& a_Dst, const PixelColor& a_Color);
/** @brief Clears the image with zeros */
void ImageClear(Image& a_Dst);
/** @brief Blits the image to the destination */
void ImageBlit(
    const Image& a_Src,
    Image& a_Dst,
    const glm::uvec3& a_SrcOffset,
    const glm::uvec3& a_DstOffset,
    const glm::uvec3& a_Size);
/** @brief Flips the image on the specified axis */
void ImageFlipX(Image& a_Dst);
/** @copydoc MSG::FlipImageX */
void ImageFlipY(Image& a_Dst);
/** @copydoc MSG::FlipImageX */
void ImageFlipZ(Image& a_Dst);
/**
 * @brief Applies the specified transform to each pixel.
 * @attention This operation will create a temporary image buffer
 *
 * @param a_TexCoordTransform a matrix to transform the pixel coordinates
 */
void ImageApplyTransform(Image& a_Dst, const glm::mat3x3& a_TexCoordTransform);
/**
 * @brief Applies a function on each pixel
 *
 * @tparam Op type of a_Op
 * @param a_Dst the image to apply treatment on
 * @param a_Op the function to apply to each pixel
 */
template <typename Op>
void ImageApplyTreatment(Image& a_Dst, const Op& a_Op)
{
    a_Dst.Map();
    for (auto z = 0u; z < a_Dst.GetSize().z; ++z) {
        for (auto y = 0u; y < a_Dst.GetSize().y; ++y) {
            for (auto x = 0u; x < a_Dst.GetSize().x; ++x) {
                a_Dst.Store({ x, y, z }, a_Op(a_Dst.Load({ x, y, z })));
            }
        }
    }
    a_Dst.Unmap();
}
}
