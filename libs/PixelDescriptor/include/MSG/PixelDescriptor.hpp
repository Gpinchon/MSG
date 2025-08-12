#pragma once

#include <MSG/Core/DataType.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/Pixel/SizedFormat.hpp>
#include <MSG/Pixel/SizedFormatHelper.hpp>

#include <glm/common.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <array>
#include <cstddef>
#include <type_traits>
#include <vector>

namespace MSG {
using PixelColor = glm::vec4;
using PixelSize  = glm::uvec3;
using PixelCoord = glm::uvec3;

PixelColor LinearToSRGB(const PixelColor& color);

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
inline auto PixelBilinearFilter(
    const float& tx,
    const float& ty,
    const PixelColor& c00,
    const PixelColor& c10,
    const PixelColor& c01,
    const PixelColor& c11)
{
    auto a = glm::mix(c00, c10, tx);
    auto b = glm::mix(c01, c11, tx);
    return glm::mix(a, b, ty);
}

float GetPixelNormalizedColorComponent(const Core::DataType& a_Type, const std::byte* a_Bytes);

float GetPixelColorComponent(const Core::DataType& a_Type, const std::byte* a_Bytes);

/**
 * @brief gets the byte size of a single channel
 * @param a_Format a sized format to query the channel size from
 * @param a_Channel a single channel to query the size of
 * @return the channel size in bytes
 */
uint8_t GetPixelChannelDataTypeSize(const PixelSizedFormat& a_Format, const PixelColorChannel& a_Channel);

struct PixelDescriptor {
    /// @return the pixel size in octets
    READONLYPROPERTY(uint8_t, PixelSize, 0);
    READONLYPROPERTY(PixelSizedFormatHelper, PixelSizedFormatHelper, );

public:
    PixelDescriptor() = default;
    PixelDescriptor(const PixelSizedFormat& a_Format);

    /**
     * @brief Converts raw bytes to float RGBA representation
     * @param bytes the raw bytes to be converted
     * @return the unpacked color
     */
    PixelColor GetColorFromBytes(const std::byte* bytes) const;
    /**
     * @brief Get color from image's raw bytes
     * @param bytes : the image's raw bytes
     * @param imageSize : image's size in pixels
     * @param pixelCoordinates : the pixel to fetch
     * @return the color of the pixel located at textureCoordinates
     */
    PixelColor GetColorFromBytes(const std::vector<std::byte>& bytes, const PixelSize& imageSize, const PixelCoord& pixelCoordinates) const;
    /**
     * @brief Writes color to the raw bytes
     * @param bytes the raw bytes to write to
     * @param color the color to write
     */
    void SetColorToBytes(std::byte* bytes, const PixelColor& color) const;
    /**
     * @brief Writes color to the raw bytes range
     * @param a_Begin the beginning of the raw bytes to write to
     * @param a_End the end of the raw bytes to write to
     * @param a_Color the color to write
     */
    void SetColorToBytesRange(std::byte* a_Begin, std::byte* a_End, const PixelColor& color) const;
    /**
     * @brief Computes the pixel index at pixelCoordinates of an image with specified imageSize
     * @param imageSize : the size of the image in pixels
     * @param pixelCoordinates : the pixel's coordinates to fetch the index for
     * @return the pixel index, the DXT block index for DXT formats
     */
    inline size_t GetPixelIndex(const PixelSize& imageSize, const PixelCoord& coord) const
    {
        if (GetDataType() == Core::DataType::DXT5) {
            // DXT5 compression format is composed of 4x4 pixels
            constexpr glm::uvec3 blockPixels = { 4, 4, 1 };
            constexpr size_t blockSize       = 16;
            const glm::uvec3 blockCount      = (imageSize + (blockPixels - 1u)) / blockPixels;
            const glm::uvec3 blockCoord      = coord / blockPixels;
            const size_t blockIndex          = (blockCoord.z * blockCount.x * blockCount.y) + (blockCoord.y * blockCount.x) + blockCoord.x;
            return blockSize * blockIndex;
        } else {
            auto unsizedPixelIndex = static_cast<size_t>((coord.z * imageSize.x * imageSize.y) + (coord.y * imageSize.x) + coord.x);
            return unsizedPixelIndex * GetPixelSize();
        }
    }
    size_t GetPixelBufferByteSize(const PixelSize& a_Size) const;
    bool operator==(const PixelDescriptor& a_Rhs) const { return GetPixelSizedFormatHelper() == a_Rhs.GetPixelSizedFormatHelper(); }
    bool operator!=(const PixelDescriptor& a_Rhs) const { return !(*this == a_Rhs); }
    bool operator!=(PixelDescriptor& a_Rhs) const { return !(*this == a_Rhs); }
    Core::DataType GetDataType(const uint8_t& a_ChannelIndex = 0) const
    {
        assert(a_ChannelIndex < 4);
        return GetPixelSizedFormatHelper().channel[a_ChannelIndex];
    }
    auto& GetSizedFormat() const { return GetPixelSizedFormatHelper().value; }
    auto& GetUnsizedFormat() const { return GetPixelSizedFormatHelper().format; }
    /// @return the pixel type size in octets
    uint8_t GetDataTypeSize(const uint8_t& a_ChannelIndex) const { return DataTypeSize(GetDataType(a_ChannelIndex)); }
    uint8_t GetComponentsNbr() const { return GetPixelComponentsNbr(GetUnsizedFormat()); }
    bool IsNormalized() const { return GetPixelType(GetUnsizedFormat()) == PixelTypeNormalized; }
    bool HasAlpha() const { return PixelHasColorChannel(GetUnsizedFormat(), PixelColorChannelAlpha); }
    /**
     * @brief compressed the specified colors into a DXT5 color block
     *
     * @param a_Colors a pointer to 16 colors
     * @return std::array<std::byte, 16>
     */
    std::array<std::byte, 16> CompressBlock(const PixelColor* a_Colors) const;
    /**
     * @brief decompresses a DXT5 color block
     *
     * @param a_Block a pointer to 16 bytes containing the DXT5 block
     * @return std::array<PixelColor, 16>
     */
    std::array<PixelColor, 16> DecompressBlock(const std::byte* a_Block) const;
    std::array<glm::u8vec4, 16> DecompressBlockToUI8(const std::byte* a_Block) const;
    typedef void (*SetComponentFunc)(std::byte* a_Bytes, const float& a_Component);
    typedef float (*GetComponentFunc)(const std::byte* a_Bytes);
    std::array<SetComponentFunc, 4> SetComponent;
    std::array<GetComponentFunc, 4> GetComponent;
};
}
