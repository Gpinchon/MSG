#pragma once

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>

namespace Msg::Renderer {
class Impl;
}

namespace Msg::Renderer {
class ShadowsSubsystem : public SubsystemInterface {
public:
    ShadowsSubsystem(Renderer::Impl& a_Renderer);
    void Update(const SubsystemsLibrary& a_Subsystems) override;

private:
    OGLFence _executionFence;
    OGLCmdBuffer _cmdBuffer;
};
}