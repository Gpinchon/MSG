#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

#include <memory>

namespace Msg::Renderer::GLSL {
struct CameraUBO;
}

namespace Msg {
template <typename>
class OGLTypedBuffer;
}

namespace Msg::Renderer {
class CameraSubsystem : public SubsystemInterface {
public:
    CameraSubsystem(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    std::shared_ptr<OGLTypedBuffer<GLSL::CameraUBO>> buffer;
};
}