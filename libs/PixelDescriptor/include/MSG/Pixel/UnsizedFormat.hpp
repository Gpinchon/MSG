#pragma once

#include <bit>
#include <cassert>
#include <cstdint>
#include <limits>

namespace Msg {
using PixelColorChannel                                = uint8_t;
constexpr PixelColorChannel PixelColorChannelRed       = 0b000001;
constexpr PixelColorChannel PixelColorChannelGreen     = 0b000010;
constexpr PixelColorChannel PixelColorChannelBlue      = 0b000100;
constexpr PixelColorChannel PixelColorChannelAlpha     = 0b001000;
constexpr PixelColorChannel PixelColorChannelDepth     = 0b010000;
constexpr PixelColorChannel PixelColorChannelStencil   = 0b100000;
constexpr PixelColorChannel PixelColorChannelDXT5Block = 0b100001; // special flag
constexpr uint64_t PixelColorChannelBitCount           = 6;
using PixelType                                        = uint8_t;
constexpr PixelType PixelTypeNormalized                = 0b1;
constexpr PixelType PixelTypeInteger                   = 0b0;

/**
 * @param a_ColorChannel a single color channel to query the index for
 */
constexpr uint8_t GetPixelChannelIndex(const PixelColorChannel& a_Channel)
{
    assert(a_Channel != PixelColorChannelDepth && a_Channel != PixelColorChannelStencil);
    return std::countr_zero(a_Channel);
}

enum class PixelUnsizedFormat : uint8_t {
    Unknown       = std::numeric_limits<uint8_t>::max(),
    R             = (PixelTypeNormalized << PixelColorChannelBitCount) | PixelColorChannelRed,
    RG            = (PixelTypeNormalized << PixelColorChannelBitCount) | PixelColorChannelRed | PixelColorChannelGreen,
    RGB           = (PixelTypeNormalized << PixelColorChannelBitCount) | PixelColorChannelRed | PixelColorChannelGreen | PixelColorChannelBlue,
    RGBA          = (PixelTypeNormalized << PixelColorChannelBitCount) | PixelColorChannelRed | PixelColorChannelGreen | PixelColorChannelBlue | PixelColorChannelAlpha,
    R_Integer     = (PixelTypeInteger << PixelColorChannelBitCount) | PixelColorChannelRed,
    RG_Integer    = (PixelTypeInteger << PixelColorChannelBitCount) | PixelColorChannelRed | PixelColorChannelGreen,
    RGB_Integer   = (PixelTypeInteger << PixelColorChannelBitCount) | PixelColorChannelRed | PixelColorChannelGreen | PixelColorChannelBlue,
    RGBA_Integer  = (PixelTypeInteger << PixelColorChannelBitCount) | PixelColorChannelRed | PixelColorChannelGreen | PixelColorChannelBlue | PixelColorChannelAlpha,
    RGBA_DXT5     = (PixelTypeNormalized << PixelColorChannelBitCount) | PixelColorChannelDXT5Block,
    Depth         = (PixelTypeNormalized << PixelColorChannelBitCount) | PixelColorChannelDepth,
    Stencil       = (PixelTypeInteger << PixelColorChannelBitCount) | PixelColorChannelStencil,
    Depth_Stencil = PixelColorChannelDepth | PixelColorChannelStencil,
};

constexpr PixelType GetPixelType(const PixelUnsizedFormat& a_Format) { return PixelType(uint16_t(a_Format) >> PixelColorChannelBitCount); }
constexpr bool PixelHasColorChannel(const PixelUnsizedFormat& a_Format, const PixelColorChannel& a_ColorChannel) { return uint8_t(a_Format) & a_ColorChannel; }
constexpr uint8_t GetPixelComponentsNbr(const PixelUnsizedFormat& a_Format) { return std::countr_one(uint8_t(uint8_t(a_Format) & 0b00111111)); }
}
