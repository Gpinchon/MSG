/*
* @Author: gpinchon
* @Date:   2021-05-30 22:26:26
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-31 12:12:39
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Core {
class Window;
}

////////////////////////////////////////////////////////////////////////////////
// Class Definition
////////////////////////////////////////////////////////////////////////////////
namespace OpenGL {
class Context {
public:
    using Handle = void*;
    Context(std::weak_ptr<TabGraph::Core::Window> window, uint8_t versionMajor, uint8_t versionMinor);
    ~Context();
    void MakeCurrent();
    inline void SetWindow(std::weak_ptr<TabGraph::Core::Window> window)
    {
        _window = window;
    }
    inline std::shared_ptr<TabGraph::Core::Window> GetWindow() const
    {
        return _window.lock();
    }
    inline Handle GetHandle() const
    {
        return _handle;
    }

private:
    Handle _handle { 0 };
    std::weak_ptr<TabGraph::Core::Window> _window;
};
}
