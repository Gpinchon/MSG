/*
 * @Author: gpinchon
 * @Date:   2021-01-12 18:26:58
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-29 18:21:49
 */
#pragma once

#include <SG/Core/DataType.hpp>
#include <SG/Core/Property.hpp>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <cstddef>
#include <vector>

namespace TabGraph::SG::Pixel {
using Color                                 = glm::vec4;
using Size                                  = glm::uvec3;
using Coord                                 = glm::uvec3;
using ColorChannel                          = uint8_t;
constexpr ColorChannel ColorChannelRed      = 0b000001;
constexpr ColorChannel ColorChannelGreen    = 0b000010;
constexpr ColorChannel ColorChannelBlue     = 0b000100;
constexpr ColorChannel ColorChannelAlpha    = 0b001000;
constexpr ColorChannel ColorChannelDepth    = 0b010000;
constexpr ColorChannel ColorChannelStencil  = 0b100000;
constexpr ColorChannel ColorChannelMaxValue = ColorChannelRed | ColorChannelGreen | ColorChannelBlue | ColorChannelAlpha | ColorChannelDepth | ColorChannelStencil;
using PixelType                             = uint8_t;
constexpr PixelType PixelTypeNormalized     = 0b1;
constexpr PixelType PixelTypeInteger        = 0b0;
constexpr PixelType PixelTypeMaxValue       = PixelTypeNormalized | PixelTypeInteger;

enum class UnsizedFormat : uint16_t {
    R             = (PixelTypeNormalized << 8) | ColorChannelRed,
    RG            = (PixelTypeNormalized << 8) | ColorChannelRed | ColorChannelGreen,
    RGB           = (PixelTypeNormalized << 8) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue,
    RGBA          = (PixelTypeNormalized << 8) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue | ColorChannelAlpha,
    R_Integer     = (PixelTypeInteger << 8) | ColorChannelRed,
    RG_Integer    = (PixelTypeInteger << 8) | ColorChannelRed | ColorChannelGreen,
    RGB_Integer   = (PixelTypeInteger << 8) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue,
    RGBA_Integer  = (PixelTypeInteger << 8) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue | ColorChannelAlpha,
    Depth         = ColorChannelDepth,
    Stencil       = ColorChannelStencil,
    Depth_Stencil = ColorChannelDepth | ColorChannelStencil,
    MaxValue      = (PixelTypeMaxValue << 8) | ColorChannelMaxValue,
    Unknown       = MaxValue + 1
};

enum class SizedFormat : uint32_t {
    /// <summary>
    /// Normalized Uint8 pixel types
    /// </summary>
    Uint8_NormalizedR    = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::R),
    Uint8_NormalizedRG   = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::RG),
    Uint8_NormalizedRGB  = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::RGB),
    Uint8_NormalizedRGBA = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::RGBA),
    /// <summary>
    /// Normalized Int8 pixel types
    /// </summary>
    Int8_NormalizedR    = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::R),
    Int8_NormalizedRG   = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::RG),
    Int8_NormalizedRGB  = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::RGB),
    Int8_NormalizedRGBA = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::RGBA),
    /// <summary>
    /// Unnormalized Uint8 pixel types
    /// </summary>
    Uint8_R    = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Uint8_RG   = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Uint8_RGB  = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Uint8_RGBA = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    /// <summary>
    /// Unnormalized Int8 pixel types
    /// </summary>
    Int8_R    = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Int8_RG   = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Int8_RGB  = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Int8_RGBA = (uint32_t(DataType::Int8) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    /// <summary>
    /// Normalized Uint16 pixel types
    /// </summary>
    Uint16_NormalizedR    = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::R),
    Uint16_NormalizedRG   = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::RG),
    Uint16_NormalizedRGB  = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::RGB),
    Uint16_NormalizedRGBA = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::RGBA),
    /// <summary>
    /// Normalized Int16 pixel types
    /// </summary>
    Int16_NormalizedR    = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::R),
    Int16_NormalizedRG   = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::RG),
    Int16_NormalizedRGB  = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::RGB),
    Int16_NormalizedRGBA = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::RGBA),
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Uint16_R    = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Uint16_RG   = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Uint16_RGB  = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Uint16_RGBA = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Int16_R    = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Int16_RG   = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Int16_RGB  = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Int16_RGBA = (uint32_t(DataType::Int16) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    /// <summary>
    /// Unnormalized Uint32 pixel types
    /// </summary>
    Uint32_R    = (uint32_t(DataType::Uint32) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Uint32_RG   = (uint32_t(DataType::Uint32) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Uint32_RGB  = (uint32_t(DataType::Uint32) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Uint32_RGBA = (uint32_t(DataType::Uint32) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    /// <summary>
    /// Unnormalized Int32 pixel types
    /// </summary>
    Int32_R    = (uint32_t(DataType::Int32) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Int32_RG   = (uint32_t(DataType::Int32) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Int32_RGB  = (uint32_t(DataType::Int32) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Int32_RGBA = (uint32_t(DataType::Int32) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    /// <summary>
    /// Floating Point pixel types
    /// </summary>
    Float16_R    = (uint32_t(DataType::Float16) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Float16_RG   = (uint32_t(DataType::Float16) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Float16_RGB  = (uint32_t(DataType::Float16) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Float16_RGBA = (uint32_t(DataType::Float16) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    Float32_R    = (uint32_t(DataType::Float32) << 16) | uint32_t(UnsizedFormat::R_Integer),
    Float32_RG   = (uint32_t(DataType::Float32) << 16) | uint32_t(UnsizedFormat::RG_Integer),
    Float32_RGB  = (uint32_t(DataType::Float32) << 16) | uint32_t(UnsizedFormat::RGB_Integer),
    Float32_RGBA = (uint32_t(DataType::Float32) << 16) | uint32_t(UnsizedFormat::RGBA_Integer),
    /// <summary>
    /// Depth pixel types
    /// </summary>
    Depth16  = (uint32_t(DataType::Uint16) << 16) | uint32_t(UnsizedFormat::Depth),
    Depth24  = (uint32_t(DataType::Uint24) << 16) | uint32_t(UnsizedFormat::Depth),
    Depth32  = (uint32_t(DataType::Uint32) << 16) | uint32_t(UnsizedFormat::Depth),
    Depth32F = (uint32_t(DataType::Float32) << 16) | uint32_t(UnsizedFormat::Depth),
    /// <summary>
    /// Stencil pixel types
    /// </summary>
    Stencil8 = (uint32_t(DataType::Uint8) << 16) | uint32_t(UnsizedFormat::Stencil),
    /// <summary>
    /// Depth/Stencil pixel types
    /// </summary>
    Depth24_Stencil8  = uint32_t(Depth24) | Stencil8,
    Depth32F_Stencil8 = uint32_t(Depth32F) | Stencil8,
    /// <summary>
    /// Compressed pixel types
    /// </summary>
    DXT5_RGBA = uint32_t(DataType::DXT5Block) << 16,
    MaxValue  = (uint32_t(DataType::MaxValue) << 16) | uint32_t(UnsizedFormat::MaxValue),
    Unknown   = MaxValue + 1
};

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

float GetNormalizedColorComponent(DataType a_Type, const std::byte* a_Bytes);

float GetColorComponent(DataType a_Type, const std::byte* a_Bytes);

SizedFormat GetSizedFormat(UnsizedFormat a_UnsizedFormat, DataType a_Type);

uint8_t GetUnsizedFormatComponentsNbr(UnsizedFormat a_Format);

uint8_t GetOctetsPerPixels(UnsizedFormat a_Format, DataType a_Type);

uint8_t GetChannelOctets(const SizedFormat& a_Format, const ColorChannel& a_Channel);

struct Description {
    READONLYPROPERTY(SizedFormat, SizedFormat, Pixel::SizedFormat::Unknown);
    READONLYPROPERTY(UnsizedFormat, UnsizedFormat, Pixel::UnsizedFormat::Unknown);
    /// @return the data type
    READONLYPROPERTY(DataType, DataType, DataType::Unknown);
    /// @return the pixel type size in octets
    READONLYPROPERTY(uint8_t, TypeSize, 0);
    /// @return the number of components
    READONLYPROPERTY(uint8_t, Components, 0);
    /// @return the pixel size in octets
    READONLYPROPERTY(uint8_t, Size, 0);
    /// @return true if the pixel is to be normalized
    READONLYPROPERTY(bool, Normalized, false);
    READONLYPROPERTY(bool, HasAlpha, false);

public:
    Description() = default;
    Description(UnsizedFormat a_Format, DataType a_Type);
    Description(SizedFormat a_Format);
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
};
}
