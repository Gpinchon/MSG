#pragma once

#include <MSG/PixelDescriptor.hpp>
#include <MSG/Tools/WorkerThread.hpp>

#include <any>
#include <functional>
#include <memory_resource>
#include <mutex>

namespace Platform {
class Ctx;
struct CtxDeleter {
    void operator()(Platform::Ctx* a_Ptr);
};
uint64_t CtxGetID(const Ctx& a_Ctx);
void CtxMakeCurrent(const Ctx& a_Ctx);
void CtxSwapBuffers(const Ctx& a_Ctx);
void CtxRelease(const Ctx& a_Ctx);
void CtxSetSwapInterval(const Ctx& a_Ctx, const int8_t& a_Interval);
}

namespace MSG::Renderer {
class Context;

struct CreateContextInfo {
    std::any nativeDisplayHandle;
    Context* sharedContext  = nullptr;
    uint8_t maxPendingTasks = 16;
    bool setPixelFormat     = true;
};

/**
 * @brief The context's command queue
 */
class ContextCmdQueue {
public:
    ContextCmdQueue(const uint32_t& a_MaxPendingTasks = 16);
    /**
     * @brief Pushes a command to the pending commands queue
     * @param a_Command the command to push
     */
    void PushCmd(const ::std::function<void()>& a_Command);
    /**
     * @brief Pushes a command that will immediatly be executed
     * @param a_Command the command to push
     * @param a_Synchronous if true, the function will return when command is executed
     */
    void PushImmediateCmd(const std::function<void()>& a_Command, const bool& a_Synchronous = false);
    void ExecuteCmds(bool a_Synchronous = false);
    bool Busy();
    void WaitWorkerThread();

    std::mutex mutex;
    uint32_t maxPendingTasks;
    Tools::WorkerThread workerThread;
    std::pmr::unsynchronized_pool_resource memoryResource {};
    std::pmr::vector<Tools::WorkerThread::Task> pendingCmds { &memoryResource };
};

/**
 * @brief Context interface for outsider
 */
class Context : public ContextCmdQueue {
public:
    Context(const CreateContextInfo& a_Info, Platform::Ctx* a_Ctx);
    Context(Context&& a_Other);
    Context(const Context&) = delete;
    ~Context();
    uint64_t GetID() const;
    void WaitGPU();
    void Release();
    std::unique_ptr<Platform::Ctx, Platform::CtxDeleter> impl;
};

template <typename ContextType>
class ContextT : public Context {
public:
    ContextT(const CreateContextInfo& a_Info)
        : Context(a_Info, new ContextType(a_Info))
    {
    }
    ContextType& GetCtx() { return *std::static_pointer_cast<ContextType>(impl); }
};
}
