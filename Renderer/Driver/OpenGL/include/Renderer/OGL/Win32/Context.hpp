#pragma once

#include <Tools/WorkerThread.hpp>

namespace TabGraph::Renderer::RAII {
struct Context {
    Context(const void* a_HWND, bool a_Offscreen);
    Context(Context&& a_Other);
    Context(const Context&) = delete;
    ~Context();
    void Release();
    void PushRenderCmd(const std::function<void()>& a_Command)
    {
        renderThread.PushCommand(a_Command);
    }
    void ExecuteRenderCmds(bool a_Synchronous = false)
    {
        renderThread.Execute(a_Synchronous);
    }
    // This will be pushed to the renderThread for now
    void PushResourceCreationCmd(const std::function<void()>& a_Command)
    {
        renderThread.PushCommand(a_Command);
    }
    void ExecuteResourceCreationCmds(bool a_Synchronous = false)
    {
        renderThread.Execute(a_Synchronous);
    }
    // This will be pushed to the renderThread for now
    void PushResourceDestructionCmd(const std::function<void()>& a_Command)
    {
        renderThread.PushCommand(a_Command);
    }
    void ExecuteResourceDestructionCmds(bool a_Synchronous = false)
    {
        renderThread.Execute(a_Synchronous);
    }
    void Wait();
    Tools::ManualWorkerThread renderThread;
    void* hwnd  = nullptr;
    void* hdc   = nullptr;
    void* hglrc = nullptr;
};
}
