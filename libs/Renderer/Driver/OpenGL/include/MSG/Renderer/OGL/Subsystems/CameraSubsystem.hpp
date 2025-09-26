#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

#include <memory>

namespace MSG::Renderer::GLSL {
struct CameraUBO;
}

namespace MSG {
template <typename>
class OGLTypedBuffer;
}

namespace MSG::Renderer {
class CameraSubsystem : public SubsystemInterface {
public:
    CameraSubsystem(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    std::shared_ptr<OGLTypedBuffer<GLSL::CameraUBO>> buffer;
};
}