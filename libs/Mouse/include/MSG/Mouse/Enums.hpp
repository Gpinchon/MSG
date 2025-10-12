#pragma once

#include <MSG/Tools/StrongTypedef.hpp>

namespace Msg::Mouse {
constexpr uint8_t LeftButton   = 0;
constexpr uint8_t MiddleButton = 1;
constexpr uint8_t RightButton  = 2;
enum class WeelDirection {
    Flipped,
    Normal
};
}
