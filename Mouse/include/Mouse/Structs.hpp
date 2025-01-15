#pragma once

#include <bitset>

namespace TabGraph::Mouse {
template <typename T>
struct Position {
    T x;
    T y;
};

struct State {
    Position<int32_t> position;
    std::bitset<8> buttons;
};
}
