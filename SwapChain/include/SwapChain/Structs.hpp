#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Renderer/Structs.hpp>

#include <any>
#include <optional>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SwapChain {
struct WindowInfo {
    bool setPixelFormat = true; // if true, will set the pixel format of the window
    Renderer::PixelFormat pixelFormat; // if setPixelFormat is true, this will be used, ignored otherwise
    std::any nativeDisplayHandle;
    std::any nativeWindowHandle;
};

struct CreateSwapChainInfo {
    bool vSync          = true;
    uint32_t width      = 0;
    uint32_t height     = 0;
    uint32_t imageCount = 1;
    WindowInfo windowInfo;
};
}
