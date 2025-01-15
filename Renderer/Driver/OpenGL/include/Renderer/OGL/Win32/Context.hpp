#pragma once

#include <Tools/WorkerThread.hpp>

#include <memory_resource>

struct HWND__;
typedef struct HWND__* HWND;

struct HDC__;
typedef struct HDC__* HDC;

struct HGLRC__;
typedef struct HGLRC__* HGLRC;

namespace TabGraph::SG::Pixel {
enum class SizedFormat : uint64_t;
}

namespace TabGraph::Renderer {
class Context {
public:
    Context(
        const HWND a_HWND,
        const bool& a_SetPixelFormat,
        const bool& a_SRGB,
        const SG::Pixel::SizedFormat& a_SizedFormat,
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
    HWND hwnd                = nullptr;
    HDC hdc                  = nullptr;
    HGLRC hglrc              = nullptr;
    Tools::WorkerThread workerThread;
    std::pmr::unsynchronized_pool_resource memoryResource;
    std::pmr::vector<Tools::WorkerThread::Task> pendingCmds { &memoryResource };
};
}
