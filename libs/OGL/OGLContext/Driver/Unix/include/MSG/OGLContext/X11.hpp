#pragma once

#include <any>
#include <memory>

namespace X11 {
class DisplayWrapper;
class WindowWrapper;

std::any CreateDefaultWindow(const std::any& a_X11Display);
std::any OpenDisplay(const char* a_DisplayName = nullptr);
void CloseDisplay(const std::any& a_Display);

class DisplayWrapper {
public:
    DisplayWrapper(const char* a_DisplayName = nullptr);
    ~DisplayWrapper();
    std::any handle;
};

class WindowWrapper {
public:
    WindowWrapper(const std::shared_ptr<DisplayWrapper>& a_Display);
    ~WindowWrapper();
    std::shared_ptr<DisplayWrapper> display;
    std::any handle;
};
}