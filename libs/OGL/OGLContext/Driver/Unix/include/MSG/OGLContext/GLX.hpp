#pragma once

#include <any>
#include <memory>

namespace X11 {
class DisplayWrapper;
class WindowWrapper;
}

namespace GLX {
class ContextWrapper {
public:
    ContextWrapper(
        const std::any& a_XDisplay,
        const std::any& a_XDrawable,
        const ContextWrapper* a_SharedContext, const bool& a_SetPixelFormat);
    ~ContextWrapper();
    std::any handleDisplay; // cannot be empty
    std::any handleDrawable; // can be empty
    std::any handle;
};

uint64_t GetID(const std::any& a_GLXContext);
void SwapBuffers(const std::any& a_XDisplay);
void Release(const std::any& a_XDisplay);
void SwapInterval(const std::any& a_XDisplay, const int8_t& a_Interval);
void MakeCurrent(const std::any& a_XDisplay, const std::any& a_XDrawable, const std::any& a_GLXContext);
void MakeCurrent(const std::any& a_XDisplay, const std::any& a_GLXContext);
}