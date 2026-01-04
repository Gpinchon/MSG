#pragma once

#include <bitset>

/**
 * @ref https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-header
 */

namespace Msg {
using DDSHeaderFlags = std::bitset<32>;
namespace DDSHeaderFlag {
    DDSHeaderFlags Caps        = 0x1;
    DDSHeaderFlags Height      = 0x2;
    DDSHeaderFlags Width       = 0x4;
    DDSHeaderFlags Pitch       = 0x8;
    DDSHeaderFlags PixelFormat = 0x1000;
    DDSHeaderFlags MipmapCount = 0x20000;
    DDSHeaderFlags LinearSize  = 0x80000;
    DDSHeaderFlags Depth       = 0x800000;
}
}
