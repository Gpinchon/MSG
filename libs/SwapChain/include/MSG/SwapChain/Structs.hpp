#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/PixelDescriptor.hpp>

#include <any>
#include <optional>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SwapChain {
struct WindowInfo {
    bool setPixelFormat = true; // if true, will set the pixel format of the window
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
