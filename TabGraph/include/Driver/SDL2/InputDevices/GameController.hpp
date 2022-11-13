/*
* @Author: gpinchon
* @Date:   2021-05-21 11:51:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:22:39
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Events/Event.hpp>
#include <Events/GameController.hpp>
#include <Events/Signal.hpp>

#include <glm/fwd.hpp>
#include <map>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
struct SDL_ControllerAxisEvent;
struct SDL_ControllerButtonEvent;
struct SDL_ControllerDeviceEvent;

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Events::GameController {
struct InputDevice : Trackable {
    InputDevice();
    ~InputDevice();
    void OpenController(int32_t index);
    void CloseController(int32_t index);
    Signal<const Event::GameControllerAxis&>& OnAxis(uint8_t index, const GameController::Axis&);
    Signal<const Event::GameControllerButton&>& OnButton(uint8_t index, const GameController::Button&);
    Signal<const Event::GameControllerButton&>& OnButtonUp(uint8_t index, const GameController::Button&);
    Signal<const Event::GameControllerButton&>& OnButtonDown(uint8_t index, const GameController::Button&);
    Signal<const Event::GameControllerDevice&>& OnConnection(uint8_t index);
    Signal<const Event::GameControllerDevice&>& OnDisconnection(uint8_t index);
    bool Rumble(uint8_t index, float lowFrequency, float highFrequency, uint32_t duration);
    bool RumbleTriggers(uint8_t index, float left_rumble, float right_rumble, uint32_t duration_ms);
    bool SetLED(uint8_t index, const glm::vec3& color);
    float GetAxis(uint8_t index, GameController::Axis);
    bool GetButton(uint8_t index, GameController::Button);

private:
    void _ProcessEvent(const Event& event);
    std::map<int32_t, Signal<const Event::GameControllerDevice&>> _onConnection;
    std::map<int32_t, Signal<const Event::GameControllerDevice&>> _onDisconnection;
    std::map<int32_t, std::array<Signal<const Event::GameControllerAxis&>, (size_t)GameController::Axis::MaxValue>> _onAxis;
    std::map<int32_t, std::array<Signal<const Event::GameControllerButton&>, (size_t)GameController::Button::MaxValue>> _onButton;
    std::map<int32_t, std::array<Signal<const Event::GameControllerButton&>, (size_t)GameController::Button::MaxValue>> _onButtonUp;
    std::map<int32_t, std::array<Signal<const Event::GameControllerButton&>, (size_t)GameController::Button::MaxValue>> _onButtonDown;
};
}
namespace TabGraph::SDL2::GameController {
TabGraph::Events::Event::GameControllerAxis CreateEventData(const SDL_ControllerAxisEvent& event);
TabGraph::Events::Event::GameControllerButton CreateEventData(const SDL_ControllerButtonEvent& event);
TabGraph::Events::Event::GameControllerDevice CreateEventData(const SDL_ControllerDeviceEvent& event);
};
