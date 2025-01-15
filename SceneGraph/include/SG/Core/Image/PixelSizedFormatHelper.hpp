#pragma once

#include <SG/Core/Image/PixelSizedFormat.hpp>

namespace TabGraph::SG::Pixel {
union SizedFormatHelper {
    constexpr SizedFormatHelper(const SizedFormat& a_SizedFormat = SizedFormat::Unknown)
        : value(a_SizedFormat) { };
    constexpr SizedFormatHelper(
        const UnsizedFormat& a_Format,
        const DataType& a_Red,
        const DataType& a_Green = DataType::None,
        const DataType& a_Blue  = DataType::None,
        const DataType& a_Alpha = DataType::None)
        : format(a_Format)
        , red(a_Red)
        , green(a_Green)
        , blue(a_Blue)
        , alpha(a_Alpha)
    {
    }
    constexpr operator auto() const { return value; }
    constexpr bool operator==(const SizedFormatHelper& a_Rhs) const { return SizedFormat(*this) == SizedFormat(a_Rhs); }
    SizedFormat value;
    struct {
        UnsizedFormat format;
        DataType red;
        DataType green;
        DataType blue;
        DataType alpha;
    };
    struct {
        UnsizedFormat format;
        DataType depth;
        DataType stencil;
    };
    struct {
        UnsizedFormat format;
        DataType channel[4];
    };
};
static_assert(SizedFormat_FormatOffset == offsetof(SizedFormatHelper, format) * 8);
static_assert(SizedFormat_RedOffset == offsetof(SizedFormatHelper, red) * 8);
static_assert(SizedFormat_GreenOffset == offsetof(SizedFormatHelper, green) * 8);
static_assert(SizedFormat_BlueOffset == offsetof(SizedFormatHelper, blue) * 8);
static_assert(SizedFormat_AlphaOffset == offsetof(SizedFormatHelper, alpha) * 8);
}
