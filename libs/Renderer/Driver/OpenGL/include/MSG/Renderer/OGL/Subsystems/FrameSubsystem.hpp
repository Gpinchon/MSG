#pragma once

#include <MSG/Renderer/OGL/SubsystemInterface.hpp>

#include <memory>

namespace MSG::Renderer::GLSL {
struct FrameInfo;
}

namespace MSG {
template <typename>
class OGLTypedBuffer;
}

namespace MSG::Renderer {
class FrameSubsystem : public SubsystemInterface {
public:
    FrameSubsystem(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    std::shared_ptr<OGLTypedBuffer<GLSL::FrameInfo>> buffer;
};
}