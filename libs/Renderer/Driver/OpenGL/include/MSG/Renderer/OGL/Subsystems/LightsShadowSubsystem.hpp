#pragma once

#include <MSG/OGLFence.hpp>
#include <MSG/Renderer/SubsystemInterface.hpp>

namespace Msg {
class OGLBindlessTextureSampler;
class OGLContext;
class OGLFrameBuffer;
template <typename>
class OGLTypedBuffer;
template <typename>
class OGLTypedBufferArray;
}

namespace Msg::Renderer::GLSL {
struct ShadowCaster;
struct Camera;
}

namespace Msg::Renderer {
class Impl;
}

namespace Msg::Renderer {
class LightsShadowSubsystem : public SubsystemInterface {
public:
    LightsShadowSubsystem(Renderer::Impl& a_Renderer);
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    uint32_t countCasters   = 0;
    uint32_t countViewports = 0;
    std::shared_ptr<OGLTypedBufferArray<GLSL::ShadowCaster>> bufferCasters;
    std::shared_ptr<OGLTypedBufferArray<GLSL::Camera>> bufferViewports;
    std::vector<std::shared_ptr<OGLBindlessTextureSampler>> textureSamplers;
};
}