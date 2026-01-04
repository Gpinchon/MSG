#pragma once

#include <D3D10ResourceDim.hpp>
#include <DDSHeaderFlag.hpp>
#include <DDSPixelFlag.hpp>
#include <DXGIFormat.hpp>

#include <cstdint>

namespace Msg {
/** @ref https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-pixelformat */
struct DDSPixelFormat {
    uint32_t size;
    DDSPixelFlags flags;
    uint8_t fourCC[4];
    uint32_t RGBBitCount;
    uint32_t RBitMask;
    uint32_t GBitMask;
    uint32_t BBitMask;
    uint32_t ABitMask;
};
/** @ref https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-header-dxt10 */
struct DDSHeaderDX10 {
    DXGIFormat dxgiFormat;
    D3D10ResourceDim resourceDimension;
    uint32_t miscFlag;
    uint32_t arraySize;
    uint32_t miscFlags2;
};
/** @ref https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-header */
struct DDSHeader {
    uint32_t size;
    DDSHeaderFlags flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitchOrLinearSize;
    uint32_t depth;
    uint32_t mipMapCount;
    uint32_t reserved1[11];
    DDSPixelFormat pixelFormat;
    uint32_t caps;
    uint32_t caps2;
    uint32_t caps3;
    uint32_t caps4;
    uint32_t reserved2;
};
using DDSMagicWord = uint32_t;
}
