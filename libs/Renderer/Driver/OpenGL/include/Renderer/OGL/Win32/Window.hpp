#pragma once

#include <string>

struct HWND__;
typedef struct HWND__* HWND;

namespace TabGraph::Renderer::RAII {
struct Window {
    Window(const std::string& a_ClassName, const std::string& a_Name);
    ~Window();
    const std::string windowClass;
    const HWND hwnd;
};
}
