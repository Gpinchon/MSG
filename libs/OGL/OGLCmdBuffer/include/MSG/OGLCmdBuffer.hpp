#pragma once

#include <MSG/OGLCmd.hpp>

#include <atomic>
#include <functional>
#include <vector>

namespace MSG {
class OGLContext;
class OGLCmdPushCmdBuffer;
struct OGLCmdBufferState;
}

namespace MSG {
enum class OGLCmdBufferStatus {
    Invalid,
    Recording,
    Ready,
    Pending,
    Executing
};
enum class OGLCmdBufferType {
    Repeat, // don't clear the commands on execute
    OneShot // buffer will be reset on execute
};
class OGLCmdBuffer {
public:
    OGLCmdBuffer(OGLContext& a_Ctx, const OGLCmdBufferType& a_Type = OGLCmdBufferType::Repeat);
    void Begin();
    template <typename T, typename... Args>
    void PushCmd(Args&&... a_Args);
    void End();
    void Execute(const bool& a_Synchronous = false);
    void Reset();

private:
    friend OGLCmdPushCmdBuffer;
    void _ExecuteSub(OGLCmdBufferState& a_ParentState);
    void _ChangeStatus(const OGLCmdBufferStatus& a_Expected, const OGLCmdBufferStatus& a_Desired);
    OGLContext& _ctx;
    std::atomic<OGLCmdBufferStatus> _status;
    OGLCmdBufferType _type;
    std::vector<OGLCmd> _cmds;
};

template <typename T, typename... Args>
inline void OGLCmdBuffer::PushCmd(Args&&... a_Args)
{
    if (_status.load() != OGLCmdBufferStatus::Recording)
        abort();
    _cmds.emplace_back(T(std::forward<Args>(a_Args)...));
}
}
