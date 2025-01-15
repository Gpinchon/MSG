#pragma once

#include <SG/Core/Image/PixelUnsizedFormat.hpp>

namespace TabGraph::SG::Pixel {
constexpr auto SizedFormat_FormatOffset  = 0;
constexpr auto SizedFormat_RedOffset     = SizedFormat_FormatOffset + sizeof(UnsizedFormat) * 8;
constexpr auto SizedFormat_GreenOffset   = SizedFormat_RedOffset + sizeof(DataType) * 8;
constexpr auto SizedFormat_BlueOffset    = SizedFormat_GreenOffset + sizeof(DataType) * 8;
constexpr auto SizedFormat_AlphaOffset   = SizedFormat_BlueOffset + sizeof(DataType) * 8;
constexpr auto SizedFormat_DepthOffset   = SizedFormat_RedOffset;
constexpr auto SizedFormat_StencilOffset = SizedFormat_GreenOffset;

constexpr uint64_t GetSizedFormatBits(
    const UnsizedFormat& a_Format,
    const DataType& a_Red,
    const DataType& a_Green = DataType::None,
    const DataType& a_Blue  = DataType::None,
    const DataType& a_Alpha = DataType::None)
{
    return uint64_t(a_Format) << SizedFormat_FormatOffset
        | uint64_t(a_Red) << SizedFormat_RedOffset
        | uint64_t(a_Green) << SizedFormat_GreenOffset
        | uint64_t(a_Blue) << SizedFormat_BlueOffset
        | uint64_t(a_Alpha) << SizedFormat_AlphaOffset;
}

enum class SizedFormat : uint64_t {
    Unknown = -1,
    /// <summary>
    /// Normalized Uint8 pixel types
    /// </summary>
    Uint8_NormalizedR    = GetSizedFormatBits(UnsizedFormat::R, DataType::Uint8),
    Uint8_NormalizedRG   = GetSizedFormatBits(UnsizedFormat::RG, DataType::Uint8, DataType::Uint8),
    Uint8_NormalizedRGB  = GetSizedFormatBits(UnsizedFormat::RGB, DataType::Uint8, DataType::Uint8, DataType::Uint8),
    Uint8_NormalizedRGBA = GetSizedFormatBits(UnsizedFormat::RGBA, DataType::Uint8, DataType::Uint8, DataType::Uint8, DataType::Uint8),
    /// <summary>
    /// Normalized Int8 pixel types
    /// </summary>
    Int8_NormalizedR    = GetSizedFormatBits(UnsizedFormat::R, DataType::Int8),
    Int8_NormalizedRG   = GetSizedFormatBits(UnsizedFormat::RG, DataType::Int8, DataType::Int8),
    Int8_NormalizedRGB  = GetSizedFormatBits(UnsizedFormat::RGB, DataType::Int8, DataType::Int8, DataType::Int8),
    Int8_NormalizedRGBA = GetSizedFormatBits(UnsizedFormat::RGBA, DataType::Int8, DataType::Int8, DataType::Int8, DataType::Int8),
    /// <summary>
    /// Unnormalized Uint8 pixel types
    /// </summary>
    Uint8_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Uint8),
    Uint8_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Uint8, DataType::Uint8),
    Uint8_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Uint8, DataType::Uint8, DataType::Uint8),
    Uint8_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Uint8, DataType::Uint8, DataType::Uint8, DataType::Uint8),
    /// <summary>
    /// Unnormalized Int8 pixel types
    /// </summary>
    Int8_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Int8),
    Int8_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Int8, DataType::Int8),
    Int8_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Int8, DataType::Int8, DataType::Int8),
    Int8_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Int8, DataType::Int8, DataType::Int8, DataType::Int8),
    /// <summary>
    /// Normalized Uint16 pixel types
    /// </summary>
    Uint16_NormalizedR    = GetSizedFormatBits(UnsizedFormat::R, DataType::Uint16),
    Uint16_NormalizedRG   = GetSizedFormatBits(UnsizedFormat::RG, DataType::Uint16, DataType::Uint16),
    Uint16_NormalizedRGB  = GetSizedFormatBits(UnsizedFormat::RGB, DataType::Uint16, DataType::Uint16, DataType::Uint16),
    Uint16_NormalizedRGBA = GetSizedFormatBits(UnsizedFormat::RGBA, DataType::Uint16, DataType::Uint16, DataType::Uint16, DataType::Uint16),
    /// <summary>
    /// Normalized Int16 pixel types
    /// </summary>
    Int16_NormalizedR    = GetSizedFormatBits(UnsizedFormat::R, DataType::Int16),
    Int16_NormalizedRG   = GetSizedFormatBits(UnsizedFormat::RG, DataType::Int16),
    Int16_NormalizedRGB  = GetSizedFormatBits(UnsizedFormat::RGB, DataType::Int16),
    Int16_NormalizedRGBA = GetSizedFormatBits(UnsizedFormat::RGBA, DataType::Int16),
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Uint16_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Uint16),
    Uint16_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Uint16, DataType::Uint16),
    Uint16_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Uint16, DataType::Uint16, DataType::Uint16),
    Uint16_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Uint16, DataType::Uint16, DataType::Uint16, DataType::Uint16),
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Int16_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Int16),
    Int16_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Int16, DataType::Int16),
    Int16_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Int16, DataType::Int16, DataType::Int16),
    Int16_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Int16, DataType::Int16, DataType::Int16, DataType::Int16),
    /// <summary>
    /// Unnormalized Uint32 pixel types
    /// </summary>
    Uint32_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Uint32),
    Uint32_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Uint32, DataType::Uint32),
    Uint32_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Uint32, DataType::Uint32, DataType::Uint32),
    Uint32_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Uint32, DataType::Uint32, DataType::Uint32, DataType::Uint32),
    /// <summary>
    /// Unnormalized Int32 pixel types
    /// </summary>
    Int32_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Int32),
    Int32_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Int32, DataType::Int32),
    Int32_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Int32, DataType::Int32, DataType::Int32),
    Int32_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Int32, DataType::Int32, DataType::Int32, DataType::Int32),
    /// <summary>
    /// Floating Point pixel types
    /// </summary>
    Float16_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Float16),
    Float16_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Float16, DataType::Float16),
    Float16_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Float16, DataType::Float16, DataType::Float16),
    Float16_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Float16, DataType::Float16, DataType::Float16, DataType::Float16),
    Float32_R    = GetSizedFormatBits(UnsizedFormat::R_Integer, DataType::Float32),
    Float32_RG   = GetSizedFormatBits(UnsizedFormat::RG_Integer, DataType::Float32, DataType::Float32),
    Float32_RGB  = GetSizedFormatBits(UnsizedFormat::RGB_Integer, DataType::Float32, DataType::Float32, DataType::Float32),
    Float32_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA_Integer, DataType::Float32, DataType::Float32, DataType::Float32, DataType::Float32),
    /// <summary>
    /// Depth pixel types
    /// </summary>
    Depth16  = GetSizedFormatBits(UnsizedFormat::Depth, DataType::Uint16),
    Depth24  = GetSizedFormatBits(UnsizedFormat::Depth, DataType::Uint24),
    Depth32  = GetSizedFormatBits(UnsizedFormat::Depth, DataType::Uint32),
    Depth32F = GetSizedFormatBits(UnsizedFormat::Depth, DataType::Float32),
    /// <summary>
    /// Stencil pixel types
    /// </summary>
    Stencil8 = GetSizedFormatBits(UnsizedFormat::Stencil, DataType::Uint8),
    /// <summary>
    /// Depth/Stencil pixel types
    /// </summary>
    Depth24_Stencil8  = GetSizedFormatBits(UnsizedFormat::Depth_Stencil, DataType::Uint24, DataType::Uint8),
    Depth32F_Stencil8 = GetSizedFormatBits(UnsizedFormat::Depth_Stencil, DataType::Float32, DataType::Uint8),
    /// <summary>
    /// Compressed pixel types
    /// </summary>
    DXT5_RGBA = GetSizedFormatBits(UnsizedFormat::RGBA, DataType::DXT5Block, DataType::DXT5Block, DataType::DXT5Block, DataType::DXT5Block),
};
}
