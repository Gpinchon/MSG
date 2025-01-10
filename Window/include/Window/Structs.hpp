#pragma once

#include <any>
#include <bitset>
#include <cstdint>
#include <string>

namespace TabGraph::Window {
using Flags                                = std::bitset<19>;
constexpr Flags FlagsNoneBits              = 0b0000000000000000000;
constexpr Flags FlagsFullscreenBits        = 0b1000000000000000000; /**< fullscreen window */
constexpr Flags FlagsShownBits             = 0b0100000000000000000; /**< window is visible */
constexpr Flags FlagsHiddenBits            = 0b0010000000000000000; /**< window is not visible */
constexpr Flags FlagsBorderlessBits        = 0b0001000000000000000; /**< no window decoration */
constexpr Flags FlagsResizableBits         = 0b0000100000000000000; /**< window can be resized */
constexpr Flags FlagsMinimizedBits         = 0b0000010000000000000; /**< window is minimized */
constexpr Flags FlagsMaximizedBits         = 0b0000001000000000000; /**< window is maximized */
constexpr Flags FlagsMouseGrabbedBits      = 0b0000000100000000000; /**< window has grabbed mouse input */
constexpr Flags FlagsInputFocusBits        = 0b0000000010000000000; /**< window has input focus */
constexpr Flags FlagsMouseFocusBits        = 0b0000000001000000000; /**< window has mouse focus */
constexpr Flags FlagsFullscreenDesktopBits = 0b1000000000100000000;
constexpr Flags FlagsAllowHighdpiBits      = 0b0000000000010000000;
constexpr Flags FlagsMouseCaptureBits      = 0b0000000000001000000; /**< window has mouse captured (unrelated to FlagsMouseGrabbedBits) */
constexpr Flags FlagsAlwaysOnTopBits       = 0b0000000000000100000; /**< window should always be above others */
constexpr Flags FlagsSkipTaskbarBits       = 0b0000000000000010000; /**< window should not be added to the taskbar */
constexpr Flags FlagsUtilityBits           = 0b0000000000000001000; /**< window should be treated as a utility window */
constexpr Flags FlagsTooltipBits           = 0b0000000000000000100; /**< window should be treated as a tooltip */
constexpr Flags FlagsPopupMenuBits         = 0b0000000000000000010; /**< window should be treated as a popup menu */
constexpr Flags FlagsKeyboardGrabbedBits   = 0b0000000000000000001; /**< window has grabbed keyboard input */

struct CreateWindowInfo {
    std::string name;
    Flags flags;
    uint32_t positionX = -1u; //-1 means centered
    uint32_t positionY = -1u; //-1 means centered
    uint32_t width     = 0;
    uint32_t height    = 0;
    bool vSync         = false;
};
}
