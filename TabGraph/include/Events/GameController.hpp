/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-03 09:49:20
*/

#pragma once

namespace TabGraph::Events::GameController {
enum class Axis {
    Unknown = -1,
    LeftX,
    LeftY,
    RightX,
    RightY,
    TriggerLeft,
    TriggerRight,
    MaxValue,
};
enum class Button {
    Unknown = -1,
    A,
    B,
    X,
    Y,
    Back,
    Guide,
    Start,
    LeftStick,
    RightStick,
    Leftshoulder,
    Rightshoulder,
    DpadUp,
    DpadDown,
    DpadLeft,
    DpadRight,
    Misc1, /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button */
    Paddle1, /* Xbox Elite paddle P1 */
    Paddle2, /* Xbox Elite paddle P3 */
    Paddle3, /* Xbox Elite paddle P2 */
    Paddle4, /* Xbox Elite paddle P4 */
    Touchpad, /* PS4/PS5 touchpad button */
    MaxValue
};
};
