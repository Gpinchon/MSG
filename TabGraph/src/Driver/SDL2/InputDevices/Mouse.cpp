/*
* @Author: gpinchon
* @Date:   2021-05-21 22:03:59
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:49
*/

#include <Driver/SDL2/InputDevices/Mouse.hpp>
#include <Events/Manager.hpp>
#include <Events/InputDevice/Mouse.hpp>
#include <Window.hpp>

#include <SDL_events.h>
#include <SDL_mouse.h> // for SDL_GetMouseState, SDL_GetRelativeMouseMode

namespace TabGraph::SDL2::Mouse {
    const static std::unordered_map<uint8_t, TabGraph::Events::Mouse::Button> s_buttonLUT {
        { SDL_BUTTON_LEFT, TabGraph::Events::Mouse::Button::Left },
        { SDL_BUTTON_MIDDLE, TabGraph::Events::Mouse::Button::Middle },
        { SDL_BUTTON_RIGHT, TabGraph::Events::Mouse::Button::Right },
        { SDL_BUTTON_X1, TabGraph::Events::Mouse::Button::X1 },
        { SDL_BUTTON_X2, TabGraph::Events::Mouse::Button::X2 }
    };
    const static auto s_reversebuttonLUT { [&]() {
        std::unordered_map<TabGraph::Events::Mouse::Button, uint8_t> map;
        for (const auto& [key, value] : s_buttonLUT)
            map[value] = key;
        return map;
    }() };
    Events::Event::MouseButton CreateEventData(const SDL_MouseButtonEvent& event)
    {
        return {
            Core::Window::Get(event.windowID),
            Events::Mouse::GetPosition(),
            s_buttonLUT.at(event.button)
        };
    }
    Events::Event::MouseWheel CreateEventData(const SDL_MouseWheelEvent& event)
    {
        return {
            Core::Window::Get(event.windowID),
            TabGraph::Events::Mouse::GetPosition(),
            event.direction == SDL_MOUSEWHEEL_FLIPPED ? TabGraph::Events::Mouse::WheelDirection::Flipped : TabGraph::Events::Mouse::WheelDirection::Natural,
            glm::ivec2(event.x, event.y)
        };
    }
    Events::Event::MouseMove CreateEventData(const SDL_MouseMotionEvent& event)
    {
        return {
            Core::Window::Get(event.windowID),
            TabGraph::Events::Mouse::GetPosition(),
            glm::ivec2(event.xrel, event.yrel)
        };
    }
}

namespace TabGraph::Events::Mouse {
InputDevice::InputDevice()
{
    Events::Manager::On(Event::Type::MouseMotion).ConnectMember(this, &InputDevice::_ProcessEvent);
    Events::Manager::On(Event::Type::MouseButtonDown).ConnectMember(this, &InputDevice::_ProcessEvent);
    Events::Manager::On(Event::Type::MouseButtonUp).ConnectMember(this, &InputDevice::_ProcessEvent);
    Events::Manager::On(Event::Type::MouseWheel).ConnectMember(this, &InputDevice::_ProcessEvent);
}
bool InputDevice::GetRelative() const
{
    return SDL_GetRelativeMouseMode();
}
void InputDevice::SetRelative(bool value)
{
    SDL_SetRelativeMouseMode(SDL_bool(value));
}
bool InputDevice::GetButtonState(Button button) const
{
    auto mask = SDL_GetMouseState(nullptr, nullptr);
    return (mask & SDL_BUTTON(TabGraph::SDL2::Mouse::s_reversebuttonLUT.at(button)));
}
glm::ivec2 InputDevice::GetPosition() const
{
    glm::ivec2 pos { 0 };
    SDL_GetMouseState(&pos.x, &pos.y);
    return pos;
}
};