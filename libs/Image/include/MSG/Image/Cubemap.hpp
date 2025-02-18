#pragma once
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Image.hpp>

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class PixelDescriptor;
}

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
    const size_t& a_Width, const size_t& a_Height,
    const Image& a_EquirectangularImage);
/**
 * @brief Constructs a cubemap image from the sides
 * @attention every image in array must be the same size
 * @param a_Sides the sides to construct from in the order specified by CubemapSide
 * @return a 2D images array representing the newly created cubemap
 */
Image CubemapFromSides(const std::array<Image, 6>& a_Sides);
/**
 * @brief Converts the UV/side to normalized sampling dir
 * @param a_UV the normalized UV coordinates
 * @param a_Side the side to sample from
 * @return the normalized cubemap sampling direction
 */
glm::vec3 CubemapUVWToSampleVec(
    const glm::vec2& a_UV,
    const CubemapSide& a_Side);
/**
 * @brief Converts the UV/side to normalized sampling dir
 * @param a_UVW 2D array normalized texture coordinate
 * @return the normalized cubemap sampling direction
 */
glm::vec3 CubemapUVWToSampleDir(const glm::vec3& a_UVW);
/**
 * @brief Converts cubemap sample vector to texture array coordinates
 * @param a_SampleVec the normalized cubemap sampling direction
 * @return glm::vec3(UV.xy, sideIndex)
 */
glm::vec3 CubemapSampleDirToUVW(const glm::vec3& a_SampleVec);

/**
 * @brief Converts a normalized cubemap sampling direction
 * to equirectangular normalized coordinates
 * @param a_SampleVec the normalized cubemap sampling direction
 * @return normalized 2D sampling coordinates
 */
glm::vec2 CubemapSampleVecToEqui(glm::vec3 a_SampleVec);
}