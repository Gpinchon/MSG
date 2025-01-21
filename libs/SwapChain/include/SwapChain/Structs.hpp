#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Image/Pixel.hpp>

#include <any>
#include <optional>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SwapChain {
struct WindowPixelFormat {
    bool sRGB                              = true;
    Core::Pixel::SizedFormat colorFormat   = Core::Pixel::SizedFormat::Uint8_NormalizedRGB;
    Core::Pixel::SizedFormat depthFormat   = Core::Pixel::SizedFormat::None;
    Core::Pixel::SizedFormat stencilFormat = Core::Pixel::SizedFormat::None;
};
struct WindowInfo {
    bool setPixelFormat = true; // if true, will set the pixel format of the window
    WindowPixelFormat pixelFormat; // if setPixelFormat is true, this will be used, ignored otherwise
    std::any nativeDisplayHandle;
    std::any nativeWindowHandle;
};

struct CreateSwapChainInfo {
    bool vSync         = true;
    uint32_t width     = 0;
    uint32_t height    = 0;
    uint8_t imageCount = 1;
    WindowInfo windowInfo;
};
}
