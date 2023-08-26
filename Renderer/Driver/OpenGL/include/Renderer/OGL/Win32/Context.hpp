#pragma once

#include <Tools/WorkerThread.hpp>

namespace TabGraph::Renderer::RAII {
struct Context {
    Context(const void* a_HWND);
    Context(Context&& a_Other);
    Context(const Context&) = delete;
    ~Context();
    void Release();
    void PushRenderCmd(const std::function<void()>& a_Command, bool a_Synchronous = false)
    {
        renderThread.PushCommand(a_Command, a_Synchronous);
    }
    // This will be pushed to the renderThread for now
    void PushResourceCreationCmd(const std::function<void()>& a_Command, bool a_Synchronous = false)
    {
        renderThread.PushCommand(a_Command, a_Synchronous);
    }
    // This will be pushed to the renderThread for now
    void PushResourceDestructionCmd(const std::function<void()>& a_Command, bool a_Synchronous = false)
    {
        renderThread.PushCommand(a_Command, a_Synchronous);
    }
    Tools::WorkerThread renderThread;
    void* hwnd  = nullptr;
    void* hdc   = nullptr;
    void* hglrc = nullptr;
};
}
