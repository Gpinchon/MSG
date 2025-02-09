#pragma once

#include <MSG/Tools/WorkerThread.hpp>

#include <any>
#include <functional>
#include <memory_resource>
#include <mutex>

namespace Platform {
class Ctx;
struct CtxDeleter {
    void operator()(Ctx* a_Ptr);
};
uint64_t CtxGetID(const Ctx& a_Ctx);
void CtxMakeCurrent(const Ctx& a_Ctx);
void CtxSwapBuffers(const Ctx& a_Ctx);
void CtxRelease(const Ctx& a_Ctx);
void CtxSetSwapInterval(const Ctx& a_Ctx, const int8_t& a_Interval);
}

namespace MSG {
class OGLContext;

struct OGLContextCreateInfo {
    std::any nativeDisplayHandle;
    std::any nativeWindowHandle;
    OGLContext* sharedContext = nullptr;
    uint8_t maxPendingTasks   = 16;
    bool setPixelFormat       = true;
};

/**
 * @brief The context's command queue
 */
class OGLContextCmdQueue {
public:
    OGLContextCmdQueue(const uint32_t& a_MaxPendingTasks = 16);
    /**
     * @brief Pushes a command to the worker thread
     * @param a_Command the command to push
     * @param a_Synchronous if true, the function will return when command is executed
     */
    void PushCmd(const Tools::WorkerThread::Task& a_Command, const bool& a_Synchronous = false);
    bool Busy();
    void WaitWorkerThread();
    std::thread::id GetThreadID() const;
    /**
     * @brief checks if worker thread id matches the specified id
     *
     * @param a_ID the ID to check, std::this_thread::get_id() by default
     * @return true if the ID matches the worker thread id,
     * @return false if the ID doesn't matches the worker thread id
     */
    bool IsContextThread(const std::thread::id& a_ID = std::this_thread::get_id()) const;

    uint32_t maxPendingTasks;
    Tools::WorkerThread workerThread;
    std::pmr::unsynchronized_pool_resource memoryResource {};
};

/**
 * @brief Context interface for outsider
 */
class OGLContext : public OGLContextCmdQueue {
public:
    OGLContext(const OGLContextCreateInfo& a_Info, Platform::Ctx* a_Ctx);
    OGLContext(OGLContext&& a_Other);
    OGLContext(const OGLContext&) = delete;
    ~OGLContext();
    uint64_t GetID() const;
    void WaitGPU();
    void Release();
    std::unique_ptr<Platform::Ctx, Platform::CtxDeleter> impl;
};

/**
 * @brief Create a Normal OGLContext object that is attached to a window
 *
 * @param a_Info the informations necessary to create the context
 * @return a new OGLContext
 */
OGLContext CreateNormalOGLContext(const OGLContextCreateInfo& a_Info);

/**
 * @brief Create an Headless OGLContext object that is not attached to a window
 *
 * @param a_Info the informations necessary to create the context
 * @return a new OGLContext
 */
OGLContext CreateHeadlessOGLContext(const OGLContextCreateInfo& a_Info);

/**
 * @brief helper to execute a command on the context wether or not we're on the context's thread
 *
 * @param a_Context the context to execute the command on
 * @param a_Command the command to execute
 * @param a_Synchronous should we wait for the command to be executed?
 */
template <typename Func>
static inline void ExecuteOGLCommand(OGLContext& a_Context, Func a_Command, const bool& a_Synchronous = false)
{
    if (a_Context.IsContextThread())
        a_Command();
    else
        a_Context.PushCmd(a_Command, a_Synchronous);
}
}
