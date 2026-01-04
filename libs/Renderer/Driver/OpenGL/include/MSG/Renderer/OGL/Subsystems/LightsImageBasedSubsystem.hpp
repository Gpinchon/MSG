#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

namespace Msg {
class OGLContext;
class OGLSampler;
class OGLBindlessTextureSampler;
template <typename>
class OGLTypedBuffer;
template <typename>
class OGLTypedBufferArray;
}

namespace Msg::Renderer::GLSL {
struct LightIBL;
}

namespace Msg::Renderer {
class Impl;
}

namespace Msg::Renderer {
class LightsImageBasedSubsystem : public SubsystemInterface {
public:
    LightsImageBasedSubsystem(Renderer::Impl& a_Renderer);
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    std::shared_ptr<OGLSampler> iblSpecSampler;
    uint32_t count = 0;
    std::vector<std::shared_ptr<OGLBindlessTextureSampler>> textureSamplers;
    std::shared_ptr<OGLTypedBufferArray<GLSL::LightIBL>> buffer;
};
}