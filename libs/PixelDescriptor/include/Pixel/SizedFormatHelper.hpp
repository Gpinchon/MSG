#pragma once

#include <Core/DataType.hpp>
#include <Pixel/SizedFormat.hpp>

namespace MSG {
union PixelSizedFormatHelper {
    constexpr PixelSizedFormatHelper(const PixelSizedFormat& a_SizedFormat = PixelSizedFormat::Unknown)
        : value(a_SizedFormat) {};
    constexpr PixelSizedFormatHelper(
        const PixelUnsizedFormat& a_Format,
        const Core::DataType& a_Red,
        const Core::DataType& a_Green = Core::DataType::None,
        const Core::DataType& a_Blue  = Core::DataType::None,
        const Core::DataType& a_Alpha = Core::DataType::None)
        : format(a_Format)
        , red(a_Red)
        , green(a_Green)
        , blue(a_Blue)
        , alpha(a_Alpha)
    {
    }
    constexpr operator auto() const { return value; }
    constexpr bool operator==(const PixelSizedFormatHelper& a_Rhs) const { return PixelSizedFormat(*this) == PixelSizedFormat(a_Rhs); }
    PixelSizedFormat value;
    struct {
        PixelUnsizedFormat format;
        Core::DataType red;
        Core::DataType green;
        Core::DataType blue;
        Core::DataType alpha;
    };
    struct {
        PixelUnsizedFormat formatDepthStencil;
        Core::DataType depth;
        Core::DataType stencil;
    };
    struct {
        PixelUnsizedFormat formatChannels;
        Core::DataType channel[4];
    };
};
static_assert(PixelSizedFormat_FormatOffset == offsetof(PixelSizedFormatHelper, format) * 8);
static_assert(PixelSizedFormat_RedOffset == offsetof(PixelSizedFormatHelper, red) * 8);
static_assert(PixelSizedFormat_GreenOffset == offsetof(PixelSizedFormatHelper, green) * 8);
static_assert(PixelSizedFormat_BlueOffset == offsetof(PixelSizedFormatHelper, blue) * 8);
static_assert(PixelSizedFormat_AlphaOffset == offsetof(PixelSizedFormatHelper, alpha) * 8);
}
