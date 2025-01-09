#pragma once

#include <any>
#include <bitset>
#include <cstdint>
#include <string>

namespace TabGraph::Window {
using Flags                    = std::bitset<16>;
constexpr Flags BorderlessBits = 0b1000;
constexpr Flags FullscreenBits = 0b0100;
constexpr Flags ResizableBits  = 0b0010;
struct CreateWindowInfo {
    std::string name;
    Flags flags;
    uint32_t positionX = -1u; //-1 means centered
    uint32_t positionY = -1u; //-1 means centered
    uint32_t width     = 0;
    uint32_t height    = 0;
    bool vSync         = false;
    std::any nativeDisplayHandle; // native display handle from Renderer
};
}
