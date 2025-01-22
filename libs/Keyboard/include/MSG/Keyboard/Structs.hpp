#pragma once

#include <MSG/Keyboard/Enums.hpp>
#include <MSG/Tools/StrongTypedef.hpp>

#include <array>
#include <bitset>

namespace MSG::Keyboard {
using Modifiers                        = std::bitset<12>;
constexpr Modifiers ModifierNoneBits   = 0b0000000000000;
constexpr Modifiers ModifierLShiftBits = 0b1000000000000;
constexpr Modifiers ModifierRShiftBits = 0b0100000000000;
constexpr Modifiers ModifierLCtrlBits  = 0b0010000000000;
constexpr Modifiers ModifierRCtrlBits  = 0b0001000000000;
constexpr Modifiers ModifierLAltBits   = 0b0000100000000;
constexpr Modifiers ModifierRAltBits   = 0b0000010000000;
constexpr Modifiers ModifierLGuiBits   = 0b0000001000000;
constexpr Modifiers ModifierRGuiBits   = 0b0000000100000;
constexpr Modifiers ModifierNumBits    = 0b0000000010000;
constexpr Modifiers ModifierCapsBits   = 0b0000000000100;
constexpr Modifiers ModifierModeBits   = 0b0000000000010;
constexpr Modifiers ModifierScrollBits = 0b0000000000001;
struct State {
    std::array<bool, size_t(ScanCode::MaxValue)> keys; // true if key is pressed
    Modifiers modifiers = ModifierNoneBits;
};
}
