#pragma once

#include <MSG/Core/DataType.hpp>
#include <MSG/Pixel/UnsizedFormat.hpp>

namespace Msg {
constexpr auto PixelSizedFormat_FormatOffset  = 0;
constexpr auto PixelSizedFormat_RedOffset     = PixelSizedFormat_FormatOffset + sizeof(PixelUnsizedFormat) * 8;
constexpr auto PixelSizedFormat_GreenOffset   = PixelSizedFormat_RedOffset + sizeof(Core::DataType) * 8;
constexpr auto PixelSizedFormat_BlueOffset    = PixelSizedFormat_GreenOffset + sizeof(Core::DataType) * 8;
constexpr auto PixelSizedFormat_AlphaOffset   = PixelSizedFormat_BlueOffset + sizeof(Core::DataType) * 8;
constexpr auto PixelSizedFormat_DepthOffset   = PixelSizedFormat_RedOffset;
constexpr auto PixelSizedFormat_StencilOffset = PixelSizedFormat_GreenOffset;

constexpr uint64_t GetPixelSizedFormatBits(
    const PixelUnsizedFormat& a_Format,
    const Core::DataType& a_Red,
    const Core::DataType& a_Green = Core::DataType::None,
    const Core::DataType& a_Blue  = Core::DataType::None,
    const Core::DataType& a_Alpha = Core::DataType::None)
{
    return uint64_t(a_Format) << PixelSizedFormat_FormatOffset
        | uint64_t(a_Red) << PixelSizedFormat_RedOffset
        | uint64_t(a_Green) << PixelSizedFormat_GreenOffset
        | uint64_t(a_Blue) << PixelSizedFormat_BlueOffset
        | uint64_t(a_Alpha) << PixelSizedFormat_AlphaOffset;
}

enum class PixelSizedFormat : uint64_t {
    Unknown = uint64_t(-1),
    None    = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA, Core::DataType::None, Core::DataType::None, Core::DataType::None, Core::DataType::None),
    /// <summary>
    /// Normalized Uint8 pixel types
    /// </summary>
    Uint8_NormalizedR    = GetPixelSizedFormatBits(PixelUnsizedFormat::R, Core::DataType::Uint8),
    Uint8_NormalizedRG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG, Core::DataType::Uint8, Core::DataType::Uint8),
    Uint8_NormalizedRGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB, Core::DataType::Uint8, Core::DataType::Uint8, Core::DataType::Uint8),
    Uint8_NormalizedRGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA, Core::DataType::Uint8, Core::DataType::Uint8, Core::DataType::Uint8, Core::DataType::Uint8),
    /// <summary>
    /// Normalized Int8 pixel types
    /// </summary>
    Int8_NormalizedR    = GetPixelSizedFormatBits(PixelUnsizedFormat::R, Core::DataType::Int8),
    Int8_NormalizedRG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG, Core::DataType::Int8, Core::DataType::Int8),
    Int8_NormalizedRGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB, Core::DataType::Int8, Core::DataType::Int8, Core::DataType::Int8),
    Int8_NormalizedRGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA, Core::DataType::Int8, Core::DataType::Int8, Core::DataType::Int8, Core::DataType::Int8),
    /// <summary>
    /// Unnormalized Uint8 pixel types
    /// </summary>
    Uint8_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Uint8),
    Uint8_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Uint8, Core::DataType::Uint8),
    Uint8_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Uint8, Core::DataType::Uint8, Core::DataType::Uint8),
    Uint8_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Uint8, Core::DataType::Uint8, Core::DataType::Uint8, Core::DataType::Uint8),
    /// <summary>
    /// Unnormalized Int8 pixel types
    /// </summary>
    Int8_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Int8),
    Int8_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Int8, Core::DataType::Int8),
    Int8_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Int8, Core::DataType::Int8, Core::DataType::Int8),
    Int8_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Int8, Core::DataType::Int8, Core::DataType::Int8, Core::DataType::Int8),
    /// <summary>
    /// Normalized Uint16 pixel types
    /// </summary>
    Uint16_NormalizedR    = GetPixelSizedFormatBits(PixelUnsizedFormat::R, Core::DataType::Uint16),
    Uint16_NormalizedRG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG, Core::DataType::Uint16, Core::DataType::Uint16),
    Uint16_NormalizedRGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB, Core::DataType::Uint16, Core::DataType::Uint16, Core::DataType::Uint16),
    Uint16_NormalizedRGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA, Core::DataType::Uint16, Core::DataType::Uint16, Core::DataType::Uint16, Core::DataType::Uint16),
    /// <summary>
    /// Normalized Int16 pixel types
    /// </summary>
    Int16_NormalizedR    = GetPixelSizedFormatBits(PixelUnsizedFormat::R, Core::DataType::Int16),
    Int16_NormalizedRG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG, Core::DataType::Int16),
    Int16_NormalizedRGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB, Core::DataType::Int16),
    Int16_NormalizedRGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA, Core::DataType::Int16),
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Uint16_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Uint16),
    Uint16_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Uint16, Core::DataType::Uint16),
    Uint16_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Uint16, Core::DataType::Uint16, Core::DataType::Uint16),
    Uint16_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Uint16, Core::DataType::Uint16, Core::DataType::Uint16, Core::DataType::Uint16),
    /// <summary>
    /// Unnormalized Uint16 pixel types
    /// </summary>
    Int16_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Int16),
    Int16_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Int16, Core::DataType::Int16),
    Int16_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Int16, Core::DataType::Int16, Core::DataType::Int16),
    Int16_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Int16, Core::DataType::Int16, Core::DataType::Int16, Core::DataType::Int16),
    /// <summary>
    /// Unnormalized Uint32 pixel types
    /// </summary>
    Uint32_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Uint32),
    Uint32_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Uint32, Core::DataType::Uint32),
    Uint32_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Uint32, Core::DataType::Uint32, Core::DataType::Uint32),
    Uint32_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Uint32, Core::DataType::Uint32, Core::DataType::Uint32, Core::DataType::Uint32),
    /// <summary>
    /// Unnormalized Int32 pixel types
    /// </summary>
    Int32_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Int32),
    Int32_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Int32, Core::DataType::Int32),
    Int32_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Int32, Core::DataType::Int32, Core::DataType::Int32),
    Int32_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Int32, Core::DataType::Int32, Core::DataType::Int32, Core::DataType::Int32),
    /// <summary>
    /// Floating Point pixel types
    /// </summary>
    Float16_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Float16),
    Float16_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Float16, Core::DataType::Float16),
    Float16_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Float16, Core::DataType::Float16, Core::DataType::Float16),
    Float16_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Float16, Core::DataType::Float16, Core::DataType::Float16, Core::DataType::Float16),
    Float32_R    = GetPixelSizedFormatBits(PixelUnsizedFormat::R_Integer, Core::DataType::Float32),
    Float32_RG   = GetPixelSizedFormatBits(PixelUnsizedFormat::RG_Integer, Core::DataType::Float32, Core::DataType::Float32),
    Float32_RGB  = GetPixelSizedFormatBits(PixelUnsizedFormat::RGB_Integer, Core::DataType::Float32, Core::DataType::Float32, Core::DataType::Float32),
    Float32_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_Integer, Core::DataType::Float32, Core::DataType::Float32, Core::DataType::Float32, Core::DataType::Float32),
    /// <summary>
    /// Depth pixel types
    /// </summary>
    Depth16  = GetPixelSizedFormatBits(PixelUnsizedFormat::Depth, Core::DataType::Uint16),
    Depth24  = GetPixelSizedFormatBits(PixelUnsizedFormat::Depth, Core::DataType::Uint24),
    Depth32  = GetPixelSizedFormatBits(PixelUnsizedFormat::Depth, Core::DataType::Uint32),
    Depth32F = GetPixelSizedFormatBits(PixelUnsizedFormat::Depth, Core::DataType::Float32),
    /// <summary>
    /// Stencil pixel types
    /// </summary>
    Stencil8 = GetPixelSizedFormatBits(PixelUnsizedFormat::Stencil, Core::DataType::Uint8),
    /// <summary>
    /// Depth/Stencil pixel types
    /// </summary>
    Depth24_Stencil8  = GetPixelSizedFormatBits(PixelUnsizedFormat::Depth_Stencil, Core::DataType::Uint24, Core::DataType::Uint8),
    Depth32F_Stencil8 = GetPixelSizedFormatBits(PixelUnsizedFormat::Depth_Stencil, Core::DataType::Float32, Core::DataType::Uint8),
    /// <summary>
    /// Compressed pixel types
    /// </summary>
    DXT5_RGBA = GetPixelSizedFormatBits(PixelUnsizedFormat::RGBA_DXT5, Core::DataType::DXT5),
};
}
