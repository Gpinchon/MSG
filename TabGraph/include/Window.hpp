/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:12:52
*/

#pragma once

#include <Events/Event.hpp>
#include <Events/Signal.hpp>

#include <glm/vec2.hpp>
#include <memory>
#include <string>

namespace TabGraph::Core {
class Window : public Events::Trackable {
public:
    class Impl;
    enum class Style {
        None = 0x1, //Window has no decoration
        Titlebar = 0x2, //Window has a titlebar
        Resize = 0x4, //Window can be resized and has a maximize button
        Close = 0x8, //Window has a close button
        Fullscreen = 0x10, //Window is shown in fullscreen mode; this style cannot be combined with others, and requires a valid video mode
        Default = 0xe //Titlebar | Resize | Close
    };
    static std::shared_ptr<Window> Create(const std::string& name, const glm::ivec2& resolution, const Style style = Style::Default);
    /**
     * @param id : the id of the Window to query
     * @return a pointer to the Window corresponding to the id, nullptr if no Window corresponds to id
    */
    static std::shared_ptr<Window> Get(uint32_t id);
    /**
     * @return the last Window who took the focus, nullptr if no Window took the focus
    */
    static std::shared_ptr<Window> GetCurrent();
    uint32_t GetId();
    void SetSize(const glm::ivec2&);
    glm::ivec2 GetSize() const;
    void SetFullscreen(const bool& fullscreen);
    void Swap();
    Impl& GetImpl();
    Events::Signal<Events::Event::Window>& OnEvent(const Events::Event::Window::Type type);

private:
    Window(const std::string& name, const glm::ivec2 resolution, const Style style);
    void _ProcessEvent(const Events::Event& event);
    std::unique_ptr<Impl> _impl;
};
}


static inline auto operator|(const TabGraph::Core::Window::Style a, const TabGraph::Core::Window::Style b)
{
    return static_cast<TabGraph::Core::Window::Style>(static_cast<int>(a) | static_cast<int>(b));
}

static inline auto operator&(const TabGraph::Core::Window::Style a, const TabGraph::Core::Window::Style b)
{
    return static_cast<TabGraph::Core::Window::Style>(static_cast<int>(a) & static_cast<int>(b));
}
