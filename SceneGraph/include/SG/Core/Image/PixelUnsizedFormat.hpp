#pragma once

#include <bit>
#include <cstdint>

namespace TabGraph::SG::Pixel {
using ColorChannel                         = uint8_t;
constexpr ColorChannel ColorChannelRed     = 0b000001;
constexpr ColorChannel ColorChannelGreen   = 0b000010;
constexpr ColorChannel ColorChannelBlue    = 0b000100;
constexpr ColorChannel ColorChannelAlpha   = 0b001000;
constexpr ColorChannel ColorChannelDepth   = 0b010000;
constexpr ColorChannel ColorChannelStencil = 0b100000;
constexpr uint64_t ColorChannelBitCount    = 6;
using PixelType                            = uint8_t;
constexpr PixelType PixelTypeNormalized    = 0b1;
constexpr PixelType PixelTypeInteger       = 0b0;

/**
 * @param a_ColorChannel a single color channel to query the index for
 */
constexpr uint8_t GetChannelIndex(const ColorChannel& a_Channel)
{
    assert(a_Channel != ColorChannelDepth && a_Channel != ColorChannelStencil);
    return std::countr_zero(a_Channel);
}

enum class UnsizedFormat : uint8_t {
    Unknown       = std::numeric_limits<uint8_t>::max(),
    R             = (PixelTypeNormalized << ColorChannelBitCount) | ColorChannelRed,
    RG            = (PixelTypeNormalized << ColorChannelBitCount) | ColorChannelRed | ColorChannelGreen,
    RGB           = (PixelTypeNormalized << ColorChannelBitCount) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue,
    RGBA          = (PixelTypeNormalized << ColorChannelBitCount) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue | ColorChannelAlpha,
    R_Integer     = (PixelTypeInteger << ColorChannelBitCount) | ColorChannelRed,
    RG_Integer    = (PixelTypeInteger << ColorChannelBitCount) | ColorChannelRed | ColorChannelGreen,
    RGB_Integer   = (PixelTypeInteger << ColorChannelBitCount) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue,
    RGBA_Integer  = (PixelTypeInteger << ColorChannelBitCount) | ColorChannelRed | ColorChannelGreen | ColorChannelBlue | ColorChannelAlpha,
    Depth         = ColorChannelDepth,
    Stencil       = ColorChannelStencil,
    Depth_Stencil = ColorChannelDepth | ColorChannelStencil,
};

constexpr PixelType GetPixelType(const UnsizedFormat& a_Format) { return PixelType(uint16_t(a_Format) >> ColorChannelBitCount); }
constexpr bool HasColorChannel(const UnsizedFormat& a_Format, const ColorChannel& a_ColorChannel) { return uint8_t(a_Format) & a_ColorChannel; }
constexpr uint8_t GetComponentsNbr(const UnsizedFormat& a_Format) { return std::countr_one(uint8_t(uint8_t(a_Format) & 0b00111111)); }
}
