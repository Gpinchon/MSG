#include <MSG/PixelDescriptor.hpp>
#include <MSG/Renderer/OGL/Context.hpp>
#include <MSG/Renderer/OGL/RAII/DebugGroup.hpp>
#include <MSG/Renderer/Structs.hpp>

#ifdef _WIN32
#elif defined(__linux__)
#include <MSG/Renderer/OGL/Unix/Context.hpp>
#endif //_WIN32

#include <GL/glew.h>
#include <format>

namespace MSG::Renderer {
ContextCmdQueue::ContextCmdQueue(const uint32_t& a_MaxPendingTasks)
    : maxPendingTasks(a_MaxPendingTasks)
{
}

void ContextCmdQueue::PushCmd(const std::function<void()>& a_Command)
{
    std::lock_guard lock(mutex);
    pendingCmds.push_back(a_Command);
}

void ContextCmdQueue::PushImmediateCmd(const std::function<void()>& a_Command, const bool& a_Synchronous)
{
    ExecuteCmds(false);
    a_Synchronous ? workerThread.PushSynchronousCommand(a_Command) : workerThread.PushCommand(a_Command);
}

void ContextCmdQueue::ExecuteCmds(bool a_Synchronous)
{
    std::lock_guard lock(mutex);
    if (pendingCmds.empty())
        return a_Synchronous ? WaitWorkerThread() : void();
    auto command = [commands = std::move(pendingCmds)] {
        for (auto& task : commands)
            task();
    };
    a_Synchronous ? workerThread.PushSynchronousCommand(command) : workerThread.PushCommand(command);
}

bool ContextCmdQueue::Busy()
{
    return workerThread.PendingTaskCount() > maxPendingTasks;
}

void ContextCmdQueue::WaitWorkerThread()
{
    workerThread.Wait();
}

Context::Context(const CreateContextInfo& a_Info, Platform::Ctx* a_Ctx)
    : ContextCmdQueue(a_Info.maxPendingTasks)
    , impl(a_Ctx, {})
{
    PushImmediateCmd([this] { Platform::CtxMakeCurrent(*impl); }, false);
}

Context::Context(Context&& a_Other)
    : ContextCmdQueue(a_Other.maxPendingTasks)
{
    a_Other.Release();
    impl = std::move(a_Other.impl);
    PushImmediateCmd([this] { Platform::CtxMakeCurrent(*impl); }, false);
}

Context::~Context()
{
    if (impl != nullptr)
        Release();
}

uint64_t Context::GetID() const
{
    return CtxGetID(*impl);
}

void Context::WaitGPU()
{
    PushImmediateCmd(
        [this] {
            auto debugGroup = MSG::Renderer::RAII::DebugGroup(std::format("Wait for context : {}", GetID()));
            auto sync       = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
            glDeleteSync(sync);
        },
        true);
}

void Context::Release()
{
    PushImmediateCmd([this] { Platform::CtxRelease(*impl); }, true);
}
}
