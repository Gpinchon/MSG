#pragma once

#include <Tools/WorkerThread.hpp>

#include <any>
#include <memory_resource>

namespace TabGraph::Renderer {
struct PixelFormat;
}

namespace TabGraph::Renderer {
struct Context {
    Context(
        void* a_X11Display,
        void* a_SharedContext,
        uint64_t a_WindowID,
        const bool& a_SetPixelFormat,
        const PixelFormat& a_PixelFormat,
        const uint32_t& a_MaxPendingTasks = 16);
    Context(
        void* a_X11Display,
        void* a_SharedContext,
        const uint32_t& a_MaxPendingTasks = 16);
    Context(Context&& a_Other);
    Context(const Context&) = delete;
    ~Context();
    void Release();
    /**
     * @brief Pushes a command to the pending commands queue
     * @param a_Command the command to push
     */
    void PushCmd(const std::function<void()>& a_Command);
    /**
     * @brief Pushes a command that will immediatly be executed
     * @param a_Command the command to push
     * @param a_Synchronous if true, the function will return when command is executed
     */
    void PushImmediateCmd(const std::function<void()>& a_Command, const bool& a_Synchronous = false);
    void ExecuteCmds(bool a_Synchronous = false);
    bool Busy();
    void WaitWorkerThread();
    void Wait();

    uint32_t maxPendingTasks = 16;
    uint64_t drawableID      = 0;
    void* display            = nullptr;
    void* context            = nullptr;
    Tools::WorkerThread workerThread;
    std::pmr::unsynchronized_pool_resource memoryResource;
    std::pmr::vector<Tools::WorkerThread::Task> pendingCmds { &memoryResource };
};
}
