/*
* @Author: gpinchon
* @Date:   2021-05-19 15:55:01
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-22 14:37:47
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Window.hpp>

#include <glm/vec2.hpp>
#include <array>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
struct SDL_WindowEvent;
struct SDL_Window;

////////////////////////////////////////////////////////////////////////////////
// Class Definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Core {
class Window::Impl {
public:
    using Handle = SDL_Window*;
    Impl(const std::string& name, const glm::ivec2& resolution, const Style style);
    Events::Signal<Events::Event::Window>& OnEvent(const Events::Event::Window::Type type);
    Handle GetHandle() const;
    uint32_t GetId() const;
    glm::ivec2 GetSize() const;
    void SetSize(const glm::ivec2& size);
    void SetFullscreen(bool fullscreen);
    void Swap();

private:
    Handle _handle { nullptr };
    std::array<Events::Signal<Events::Event::Window>, size_t(Events::Event::Window::Type::MaxValue)> _onEvent;
};
}

namespace TabGraph::SDL2::Window {
    TabGraph::Events::Event::Window CreateEventData(const SDL_WindowEvent& event);
}
