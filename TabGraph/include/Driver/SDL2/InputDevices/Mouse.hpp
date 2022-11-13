/*
* @Author: gpinchon
* @Date:   2021-05-21 22:04:13
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:54
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Events/Event.hpp>
#include <Events/Mouse.hpp>
#include <Events/Signal.hpp>

#include <array>
#include <stdexcept>

struct SDL_MouseButtonEvent;
struct SDL_MouseWheelEvent;
struct SDL_MouseMotionEvent;

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Events::Mouse {
struct InputDevice : Trackable {
    InputDevice();
    ///@return true if mouse is in relative motion mode
    bool GetRelative() const;
    void SetRelative(bool relative);
    ///@return true if button us pressed
    bool GetButtonState(Button) const;
    ///@return the current mouse position in pixels
    glm::ivec2 GetPosition() const;
    Signal<const Event::MouseMove&> onMove {};
    Signal<const Event::MouseWheel&> onWheel {};
    std::array<Signal<const Event::MouseButton&>, size_t(Mouse::Button::MaxValue)> onButtonDown { {} };
    std::array<Signal<const Event::MouseButton&>, size_t(Mouse::Button::MaxValue)> onButtonUp { {} };
    std::array<Signal<const Event::MouseButton&>, size_t(Mouse::Button::MaxValue)> onButton { {} };

private:
    void _ProcessEvent(const Event& event)
    {
        switch (event.type) {
        case Event::Type::MouseMotion: {
            onMove(event.Get<Event::MouseMove>());
            break;
        }
        case Event::Type::MouseButtonDown: {
            const auto& eventData { event.Get<Event::MouseButton>() };
            onButtonDown.at(size_t(eventData.button))(eventData);
            onButton.at(size_t(eventData.button))(eventData);
            break;
        }

        case Event::Type::MouseButtonUp: {
            const auto& eventData { event.Get<Event::MouseButton>() };
            onButtonUp.at(size_t(eventData.button))(eventData);
            onButton.at(size_t(eventData.button))(eventData);
            break;
        }
        case Event::Type::MouseWheel: {
            onWheel(event.Get<Event::MouseWheel>());
            break;
        }
        default:
            throw std::runtime_error("Incorrect Event Type");
        }
    }
};
};

namespace TabGraph::SDL2::Mouse {
TabGraph::Events::Event::MouseButton CreateEventData(const SDL_MouseButtonEvent& event);
TabGraph::Events::Event::MouseWheel CreateEventData(const SDL_MouseWheelEvent& event);
TabGraph::Events::Event::MouseMove CreateEventData(const SDL_MouseMotionEvent& event);
}
