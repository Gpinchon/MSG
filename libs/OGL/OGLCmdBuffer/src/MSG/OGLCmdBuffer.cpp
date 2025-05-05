#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLCmdBufferState.hpp>
#include <MSG/OGLContext.hpp>

#include <GL/glew.h>
#include <iostream>
#include <stdexcept>
#include <vector>

MSG::OGLCmdBuffer::OGLCmdBuffer(OGLContext& a_Ctx, const OGLCmdBufferType& a_Type)
    : _ctx(a_Ctx)
    , _type(a_Type)
    , _status(OGLCmdBufferStatus::Invalid) { };

void MSG::OGLCmdBuffer::Begin()
{
    _ChangeStatus(OGLCmdBufferStatus::Invalid, OGLCmdBufferStatus::Recording);
}

void MSG::OGLCmdBuffer::End()
{
    _ChangeStatus(OGLCmdBufferStatus::Recording, OGLCmdBufferStatus::Ready);
}

void MSG::OGLCmdBuffer::Execute(const bool& a_Synchronous)
{
    if (_cmds.empty())
        return;
    _ChangeStatus(OGLCmdBufferStatus::Ready, OGLCmdBufferStatus::Pending);
    auto cmdsFunctor = [this]() mutable {
        OGLCmdBufferState state;
        _ExecuteSub(state);
    };
    ExecuteOGLCommand(_ctx, std::move(cmdsFunctor), a_Synchronous);
}

void MSG::OGLCmdBuffer::Reset()
{
    _cmds.clear();
    _status.store(OGLCmdBufferStatus::Invalid);
};

void MSG::OGLCmdBuffer::_ExecuteSub(OGLCmdBufferState& a_ParentState)
{
    _ChangeStatus(OGLCmdBufferStatus::Pending, OGLCmdBufferStatus::Executing);
    for (auto& cmd : _cmds)
        std::visit([&a_ParentState](auto& cmd) mutable { cmd(a_ParentState); }, cmd);
    if (_type == OGLCmdBufferType::OneShot) {
        _cmds.clear();
        _ChangeStatus(OGLCmdBufferStatus::Executing, OGLCmdBufferStatus::Invalid);
    } else {
        _ChangeStatus(OGLCmdBufferStatus::Executing, OGLCmdBufferStatus::Ready);
    }
}

void MSG::OGLCmdBuffer::_ChangeStatus(const OGLCmdBufferStatus& a_Expected, const OGLCmdBufferStatus& a_Desired)
{
    OGLCmdBufferStatus expected = a_Expected;
    if (!_status.compare_exchange_strong(expected, a_Desired))
        abort();
}