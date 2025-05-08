#pragma once

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
class ShadowsSubsystem : public SubsystemInterface {
public:
    ShadowsSubsystem(Renderer::Impl& a_Renderer);
    void Update(const SubsystemLibrary& a_Subsystems) override;

private:
    OGLFence _executionFence;
    OGLCmdBuffer _cmdBuffer;
};
}