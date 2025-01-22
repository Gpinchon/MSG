#pragma once

#include <Tools/WorkerThread.hpp>

#include <any>
#include <memory_resource>

typedef struct __GLXcontextRec* GLXContext;
typedef struct _XDisplay Display;

namespace MSG::Core::Pixel {
enum class SizedFormat : uint64_t;
}

namespace MSG::Renderer {
struct Context {
    Context(
        Display* a_Display,
        GLXContext a_SharedContext,
        uint64_t a_WindowID,
        const bool& a_SetPixelFormat,
        const bool& a_SRGB,
        const PixelSizedFormat& a_PixelFormat,
        const uint32_t& a_MaxPendingTasks = 16);
    Context(
        Display* a_Display,
        GLXContext a_SharedContext,
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
    Display* display         = nullptr;
    GLXContext context       = nullptr;
    Tools::WorkerThread workerThread;
    std::pmr::unsynchronized_pool_resource memoryResource;
    std::pmr::vector<Tools::WorkerThread::Task> pendingCmds { &memoryResource };
};
}
