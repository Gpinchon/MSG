#pragma once

#include <bitset>

namespace Msg {
using DDSPixelFlags = std::bitset<32>;
namespace DDSPixelFlag {
    DDSPixelFlags AlphaPixels = 0x1;
    DDSPixelFlags Alpha       = 0x2;
    DDSPixelFlags FourCC      = 0x4;
    DDSPixelFlags RGB         = 0x40;
    DDSPixelFlags YUV         = 0x200;
    DDSPixelFlags Luminance   = 0x20000;
}
}
