#pragma once

#include <Tools/WorkerThread.hpp>

namespace TabGraph::Renderer {
struct PixelFormat;
}

namespace TabGraph::Renderer::RAII {
struct Context {
    Context(
        const void* a_HWND,
        const bool& a_SetPixelFormat,
        const PixelFormat& a_PixelFormat,
        const bool& a_Offscreen,
        const uint32_t& a_MaxPendingTasks = 16);
    Context(Context&& a_Other);
    Context(const Context&) = delete;
    ~Context();
    void Release();
    /**
     * @brief Pushes a command to the pending commands queue
     * @param a_Command the command to push
    */
    void PushCmd(const std::function<void()>& a_Command)
    {
        pendingCmds.push_back(a_Command);
    }
    /**
     * @brief Pushes a command that will immediatly be executed
     * @param a_Command the command to push
     * @param a_Synchronous if true, the function will return when command is executed
    */
    void PushImmediateCmd(const std::function<void()>& a_Command, const bool& a_Synchronous = false)
    {
        if (a_Synchronous)
            workerThread.PushSynchronousCommand(a_Command);
        else
            workerThread.PushCommand(a_Command);
    }
    void ExecuteCmds(bool a_Synchronous = false)
    {
        if (pendingCmds.empty())
            return;
        auto command = [commands = std::move(pendingCmds)] {
            for (auto& task : commands)
                task();
        };
        a_Synchronous ? workerThread.PushSynchronousCommand(command) : workerThread.PushCommand(command);
    }
    bool Busy()
    {
        return workerThread.PendingTaskCount() > maxPendingTasks;
    }
    void WaitWorkerThread() {
        workerThread.Wait();
    }
    void Wait();

    uint32_t maxPendingTasks = 16;
    void* hwnd  = nullptr;
    void* hdc   = nullptr;
    void* hglrc = nullptr;
    Tools::WorkerThread workerThread;
    std::vector<Tools::WorkerThread::Task> pendingCmds;
};
}
