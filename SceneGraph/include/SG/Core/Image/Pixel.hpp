/*
 * @Author: gpinchon
 * @Date:   2021-01-12 18:26:58
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-29 18:21:49
 */
#pragma once

#include <SG/Core/DataType.hpp>
#include <SG/Core/Image/PixelSizedFormat.hpp>
#include <SG/Core/Image/PixelSizedFormatHelper.hpp>
#include <SG/Core/Property.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstddef>
#include <type_traits>
#include <vector>

namespace TabGraph::SG::Pixel {
using Color = glm::vec4;
using Size  = glm::uvec3;
using Coord = glm::uvec3;

Color LinearToSRGB(const Color& color);

/**
 * @brief Performes bilinear filtering on the supplied colors
 * @param tx : texture coordinates fract on x axis
 * @param ty : texture coordinates fract on y axis
 * @param c00 : color at pixel [x, y]
 * @param c10 : color at pixel [x + 1, y]
 * @param c01 : color at pixel [x, y + 1]
 * @param c11 : color at pixel [x + 1, y + 1]
 * @return
 */
inline auto BilinearFilter(
    const float& tx,
    const float& ty,
    const Color& c00,
    const Color& c10,
    const Color& c01,
    const Color& c11)
{
    auto a = c00 * (1 - tx) + c10 * tx;
    auto b = c01 * (1 - tx) + c11 * tx;
    return a * (1 - ty) + b * ty;
}

float GetNormalizedColorComponent(const DataType& a_Type, const std::byte* a_Bytes);

float GetColorComponent(const DataType& a_Type, const std::byte* a_Bytes);

/**
 * @brief gets the byte size of a single channel
 * @param a_Format a sized format to query the channel size from
 * @param a_Channel a single channel to query the size of
 * @return the channel size in bytes
 */
uint8_t GetChannelDataTypeSize(const SizedFormat& a_Format, const ColorChannel& a_Channel);

struct Description {
    /// @return the pixel size in octets
    READONLYPROPERTY(uint8_t, Size, 0);

public:
    Description() = default;
    Description(const SizedFormat& a_Format);

    /**
     * @brief Converts raw bytes to float RGBA representation
     * @param bytes the raw bytes to be converted
     * @return the unpacked color
     */
    Color GetColorFromBytes(const std::byte* bytes) const;
    /**
     * @brief Get color from image's raw bytes
     * @param bytes : the image's raw bytes
     * @param imageSize : image's size in pixels
     * @param pixelCoordinates : the pixel to fetch
     * @return the color of the pixel located at textureCoordinates
     */
    Color GetColorFromBytes(const std::vector<std::byte>& bytes, const Size& imageSize, const Coord& pixelCoordinates) const;
    /**
     * @brief Writes color to the raw bytes
     * @param bytes the raw bytes to write to
     * @param color the color to write
     */
    void SetColorToBytes(std::byte* bytes, const Color& color) const;
    /**
     * @brief Computes the pixel index at pixelCoordinates of an image with specified imageSize
     * @param imageSize : the size of the image in pixels
     * @param pixelCoordinates : the pixel's coordinates to fetch the index for
     * @return the pixel index, the DXT block index for DXT formats
     */
    inline size_t GetPixelIndex(const Size& imageSize, const Coord& coord) const
    {
        auto unsizedPixelIndex = static_cast<size_t>((coord.z * imageSize.x * imageSize.y) + (coord.y * imageSize.x) + coord.x);
        if (GetDataType() == DataType::DXT5Block) {
            // DXT5 compression format is composed of 4x4 pixels
            auto blockNumX    = imageSize[0] / 4;
            auto blockX       = coord[0] / 4;
            auto blockY       = coord[1] / 4;
            unsizedPixelIndex = static_cast<size_t>(blockY) * blockNumX + blockX;
        }
        return unsizedPixelIndex * GetSize();
    }
    bool operator==(const Description& other) const
    {
        return GetSizedFormat() == other.GetSizedFormat()
            && GetUnsizedFormat() == other.GetUnsizedFormat()
            && GetDataType() == other.GetDataType();
    }
    bool operator!=(const Description& other) const
    {
        return !(*this == other);
    }
    bool operator!=(Description& other) const
    {
        return !(*this == other);
    }
    DataType GetDataType(const uint8_t& a_ChannelIndex = 0) const
    {
        assert(a_ChannelIndex < 4);
        return sizedFormatHelper.channel[a_ChannelIndex];
    }
    auto& GetSizedFormat() const { return sizedFormatHelper.value; }
    auto& GetUnsizedFormat() const { return sizedFormatHelper.format; }
    /// @return the pixel type size in octets
    uint8_t GetDataTypeSize(const uint8_t& a_ChannelIndex) const { return DataTypeSize(GetDataType(a_ChannelIndex)); }
    uint8_t GetComponentsNbr() const { return Pixel::GetComponentsNbr(GetUnsizedFormat()); }
    bool IsNormalized() const { return GetPixelType(GetUnsizedFormat()) == PixelTypeNormalized; }
    bool HasAlpha() const { return HasColorChannel(GetUnsizedFormat(), ColorChannelAlpha); }
    SizedFormatHelper sizedFormatHelper;
};
}
