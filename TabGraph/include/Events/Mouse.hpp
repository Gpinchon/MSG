/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:27
*/

#pragma once

namespace TabGraph::Events::Mouse {
enum class WheelDirection {
    Unknown = -1,
    Natural, Flipped,
    MaxValue
};
enum class Button {
    Unknown = -1,
    Left, Middle, Right,
    X1, X2,
    MaxValue
};
}
