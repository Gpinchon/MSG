#pragma once

#include <string>

namespace TabGraph::Renderer::RAII {
struct Window {
    Window(const std::string& a_ClassName, const std::string& a_Name);
    ~Window();
    const std::string windowClass;
    const void* hwnd;
};
}
