#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <any>
#include <optional>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
enum class RendererMode {
    Forward,
    Deferred
};

struct PixelFormat {
    bool sRGB           = true;
    uint8_t redBits     = 8;
    uint8_t greenBits   = 8;
    uint8_t blueBits    = 8;
    uint8_t alphaBits   = 0;
    uint8_t depthBits   = 0;
    uint8_t stencilBits = 0;
};

// This is used to pass settings to Renderer at construction and during runtime
struct RendererSettings {
    float scale       = 1.f;
    bool enableTAA    = true;
    RendererMode mode = RendererMode::Forward;
};

struct CreateRendererInfo {
    std::string name            = "";
    uint32_t applicationVersion = 0;
};

struct CreateRenderBufferInfo {
    uint32_t width  = 0;
    uint32_t height = 0;
};

struct WindowInfo {
    bool setPixelFormat = true; // if true, will set the pixel format of the window
    PixelFormat pixelFormat; // if setPixelFormat is true, this will be used, ignored otherwise
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
