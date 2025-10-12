#pragma once

#include <MSG/OGLCmd.hpp>

#include <atomic>
#include <functional>
#include <vector>

namespace Msg {
class OGLFence;
class OGLContext;
class OGLCmdPushCmdBuffer;
struct OGLCmdBufferExecutionState;
}

namespace Msg {
enum class OGLCmdBufferState {
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
    void Execute(OGLFence* a_Fence = nullptr);
    void Reset();

private:
    friend OGLCmdPushCmdBuffer;
    void _ExecuteSub(OGLCmdBufferExecutionState& a_ParentState);
    void _ChangeState(const OGLCmdBufferState& a_Expected, const OGLCmdBufferState& a_Desired);
    OGLContext& _ctx;
    const OGLCmdBufferType _type;
    std::atomic<OGLCmdBufferState> _status;
    std::vector<OGLCmd> _cmds;
};

template <typename T, typename... Args>
inline void OGLCmdBuffer::PushCmd(Args&&... a_Args)
{
    if (_status.load() != OGLCmdBufferState::Recording)
        abort();
    _cmds.emplace_back(T(std::forward<Args>(a_Args)...));
}
}
