#include <MSG/Debug.hpp>
#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLCmdBufferExecutionState.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/OGLPipeline.hpp>

#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <vector>

constexpr auto GetStateStr(const MSG::OGLCmdBufferState& a_Status)
{
    constexpr std::array<const char*, 5> strLUT {
        "Invalid",
        "Recording",
        "Ready",
        "Pending",
        "Executing",
    };
    return strLUT.at(int(a_Status));
}

MSG::OGLCmdBuffer::OGLCmdBuffer(OGLContext& a_Ctx, const OGLCmdBufferType& a_Type)
    : _ctx(a_Ctx)
    , _type(a_Type)
    , _status(OGLCmdBufferState::Invalid) { };

void MSG::OGLCmdBuffer::Begin()
{
    _ChangeState(OGLCmdBufferState::Invalid, OGLCmdBufferState::Recording);
}

void MSG::OGLCmdBuffer::End()
{
    _ChangeState(OGLCmdBufferState::Recording, OGLCmdBufferState::Ready);
}

void MSG::OGLCmdBuffer::Execute(OGLFence* a_Fence)
{
    if (_cmds.empty()) {
        if (a_Fence != nullptr)
            a_Fence->Signal();
        return;
    }
    _ChangeState(OGLCmdBufferState::Ready, OGLCmdBufferState::Pending);
    auto cmdsFunctor = [this, fence = a_Fence]() mutable {
        OGLCmdBufferExecutionState state;
        _ExecuteSub(state);
        checkErrorFatal(state.renderPass.has_value(), "OGLCmdEndRenderpass not called after OGLCmdBegin/PushRenderpass!");
        if (state.pipeline.has_value())
            std::visit(
                [](auto& a_Pipeline) { a_Pipeline.Restore(); },
                *state.pipeline);
        if (fence != nullptr)
            fence->Signal();
    };
    ExecuteOGLCommand(_ctx, std::move(cmdsFunctor));
}

void MSG::OGLCmdBuffer::Reset()
{
    _cmds.clear();
    _status.store(OGLCmdBufferState::Invalid);
};

void MSG::OGLCmdBuffer::_ExecuteSub(OGLCmdBufferExecutionState& a_ParentState)
{
    _ChangeState(OGLCmdBufferState::Pending, OGLCmdBufferState::Executing);
    for (auto& cmd : _cmds)
        std::visit([&a_ParentState](auto& cmd) mutable { cmd(a_ParentState); }, cmd);
    if (_type == OGLCmdBufferType::OneShot) {
        _cmds.clear();
        _ChangeState(OGLCmdBufferState::Executing, OGLCmdBufferState::Invalid);
    } else {
        _ChangeState(OGLCmdBufferState::Executing, OGLCmdBufferState::Ready);
    }
}

void MSG::OGLCmdBuffer::_ChangeState(const OGLCmdBufferState& a_Expected, const OGLCmdBufferState& a_Desired)
{
    OGLCmdBufferState expected = a_Expected;
    if (!_status.compare_exchange_strong(expected, a_Desired)) {
        errorStream << "Could not change cmd buffer status from " << GetStateStr(a_Expected) << " to " << GetStateStr(a_Desired)
                    << " current status is " << GetStateStr(expected);
        abort();
    }
}