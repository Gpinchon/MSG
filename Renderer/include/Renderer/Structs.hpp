#pragma once

#include <string>

namespace TabGraph::Renderer {
#ifdef WIN32
struct PixelFormat {
    bool sRGB           = true;
    uint8_t colorBits   = 24;
    uint8_t alphaBits   = 0;
    uint8_t depthBits   = 0;
    uint8_t stencilBits = 0;
};
#endif //WIN32

struct CreateRendererInfo {
    std::string name { "" };
    uint32_t applicationVersion { 0 };
};
struct CreateRenderBufferInfo {
    uint32_t width = 0, height = 0;
};
struct CreateSwapChainInfo {
    bool vSync { true };
    uint32_t width { 0 }, height { 0 };
    uint32_t imageCount { 1 };
#ifdef WIN32
    bool setPixelFormat { true }; // if true, will set the pixel format of the window
    PixelFormat pixelFormat; // if setPixelFormat is true, this will be used, ignored otherwise
    void* hwnd { nullptr }; // ignored when used with ReCreate
#endif //WIN32
};
}
