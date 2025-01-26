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
enum class PresentMode {
    Immediate, // disables v-sync, no Present requests can accumulate
    MailBox, // enables v-sync, one Present request can accumulate
    FIFO, // enables v-sync, up to imageCount Present requests can accumulate
    FIFORelaxed // enables adaptative v-sync, up to imageCount Present requests can accumulate
};

struct WindowInfo {
    bool setPixelFormat = true; // if true, will set the pixel format of the window
    std::any nativeDisplayHandle;
    std::any nativeWindowHandle;
};

struct CreateSwapChainInfo {
    PresentMode presentMode = PresentMode::Immediate;
    uint32_t width          = 0;
    uint32_t height         = 0;
    uint8_t imageCount      = 3;
    WindowInfo windowInfo;
};
}
