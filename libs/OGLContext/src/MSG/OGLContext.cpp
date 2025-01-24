#include <MSG/OGLContext.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <GL/glew.h>
#include <format>

namespace MSG {
OGLContextCmdQueue::OGLContextCmdQueue(const uint32_t& a_MaxPendingTasks)
    : maxPendingTasks(a_MaxPendingTasks)
{
}

void OGLContextCmdQueue::PushCmd(const std::function<void()>& a_Command)
{
    std::lock_guard lock(mutex);
    pendingCmds.push_back(a_Command);
}

void OGLContextCmdQueue::PushImmediateCmd(const std::function<void()>& a_Command, const bool& a_Synchronous)
{
    ExecuteCmds(false);
    a_Synchronous ? workerThread.PushSynchronousCommand(a_Command) : workerThread.PushCommand(a_Command);
}

void OGLContextCmdQueue::ExecuteCmds(bool a_Synchronous)
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

bool OGLContextCmdQueue::Busy()
{
    return workerThread.PendingTaskCount() > maxPendingTasks;
}

void OGLContextCmdQueue::WaitWorkerThread()
{
    workerThread.Wait();
}

OGLContext::OGLContext(const OGLContextCreateInfo& a_Info, Platform::Ctx* a_Ctx)
    : OGLContextCmdQueue(a_Info.maxPendingTasks)
    , impl(a_Ctx, {})
{
    PushImmediateCmd([this] { Platform::CtxMakeCurrent(*impl); }, false);
}

OGLContext::OGLContext(OGLContext&& a_Other)
    : OGLContextCmdQueue(a_Other.maxPendingTasks)
{
    a_Other.Release();
    impl = std::move(a_Other.impl);
    PushImmediateCmd([this] { Platform::CtxMakeCurrent(*impl); }, false);
}

OGLContext::~OGLContext()
{
    if (impl != nullptr)
        Release();
}

uint64_t OGLContext::GetID() const
{
    return CtxGetID(*impl);
}

void OGLContext::WaitGPU()
{
    PushImmediateCmd(
        [this] {
#ifndef NDEBUG
            std::string dbgGroupMsg = std::format("Wait for context : {}", GetID());
            glPushDebugGroup(
                GL_DEBUG_SOURCE_APPLICATION,
                std::hash<std::string> {}(dbgGroupMsg),
                dbgGroupMsg.size(), dbgGroupMsg.c_str());
#endif
            auto sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
            glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
            glDeleteSync(sync);
#ifndef NDEBUG
            glPopDebugGroup();
#endif
        },
        true);
}

void OGLContext::Release()
{
    PushImmediateCmd([this] { Platform::CtxRelease(*impl); }, true);
}
}
