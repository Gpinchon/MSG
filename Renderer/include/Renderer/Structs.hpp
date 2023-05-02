#pragma once

#include <string>

namespace TabGraph::Renderer {
struct CreateRendererInfo {
    std::string name{ "" };
    uint32_t applicationVersion{ 0 };
};
struct CreateRenderBufferInfo {
    uint32_t width = 0, height = 0;
};
struct CreateSwapChainInfo {
    bool vSync{ true };
    uint32_t width{ 0 }, height{ 0 };
    uint32_t imageCount{ 1 };
#ifdef WIN32
    void* hwnd{ nullptr };
#endif
};
}