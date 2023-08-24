#pragma once

#include <Renderer/Handles.hpp>

#include <Tools/WorkerThread.hpp>

#include <GL/glew.h>
#include <string>

namespace TabGraph::Renderer {
struct CreateRendererInfo;
struct Impl {
    Impl(const CreateRendererInfo& a_Info);
    ~Impl();
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
    uint32_t version;
    std::string name;
    Tools::WorkerThread renderThread;
#ifdef _WIN32
    std::string windowClassName;
    void* window; // HWND
    void* displayContext; // HDC
    void* renderContext; // HGLRC
#endif
};
}
