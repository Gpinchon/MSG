#include <MSG/OGLContext.hpp>

#include <GL/glew.h>
#include <format>

namespace MSG {
OGLContextCmdQueue::OGLContextCmdQueue(const uint32_t& a_MaxPendingTasks)
    : maxPendingTasks(a_MaxPendingTasks)
{
}

void OGLContextCmdQueue::PushCmd(const std::function<void()>& a_Command, const bool& a_Synchronous)
{
    a_Synchronous ? workerThread.PushSynchronousCommand(a_Command) : workerThread.PushCommand(a_Command);
}

bool OGLContextCmdQueue::Busy()
{
    return workerThread.PendingTaskCount() > maxPendingTasks;
}

void OGLContextCmdQueue::WaitWorkerThread()
{
    workerThread.Wait();
}

std::thread::id OGLContextCmdQueue::GetThreadID() const
{
    return workerThread.GetId();
}

bool OGLContextCmdQueue::IsContextThread(const std::thread::id& a_ID) const
{
    return a_ID == workerThread.GetId();
}

OGLContext::OGLContext(const OGLContextCreateInfo& a_Info, Platform::Ctx* a_Ctx)
    : OGLContextCmdQueue(a_Info.maxPendingTasks)
    , impl(a_Ctx, {})
{
    PushCmd([this] { Platform::CtxMakeCurrent(*impl); }, false);
}

OGLContext::OGLContext(OGLContext&& a_Other)
    : OGLContextCmdQueue(a_Other.maxPendingTasks)
{
    a_Other.Release();
    impl = std::move(a_Other.impl);
    PushCmd([this] { Platform::CtxMakeCurrent(*impl); }, false);
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
    PushCmd(
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
    PushCmd([this] { Platform::CtxRelease(*impl); }, true);
}
}
