#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLFence.hpp>

#include <Bindings.glsl>
#include <SDF.glsl>

#include <memory>

namespace Msg {
class Scene;
class OGLTexture3D;
class OGLContext;
class OGLProgram;
class OGLSampler;
template <typename>
class OGLTypedBuffer;
template <typename>
class OGLTypedBufferArray;
};

namespace Msg::Renderer::GLSL {
struct FogArea;
struct FogSettings;
struct FogCamera;
}

namespace Msg::Renderer {
class Impl;
}

namespace Msg::Renderer {
union VolumetricFogShape {
    GLSL::Cube cube;
    GLSL::Sphere sphere;
};

struct VolumetricFogCascadeTextures {
    glm::uvec3 resolution;
    /// @brief RGB: Scattering, A: Extinction
    std::shared_ptr<OGLTexture3D> participatingMediaTexture0;
    /// @brief RGB: Emissive, A: Phase(g)
    std::shared_ptr<OGLTexture3D> participatingMediaTexture1;
    /// @brief RGB: Scattered light to camera, A: Extinction
    std::shared_ptr<OGLTexture3D> scatteringTexture;
    std::shared_ptr<OGLTexture3D> scatTransTexture;
    std::shared_ptr<OGLTexture3D> resultTexture;
    std::shared_ptr<OGLTexture3D> resultTexture_Previous;
};

class FogSubsystem : public SubsystemInterface {
public:
    FogSubsystem(Renderer::Impl& a_Renderer);
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    void UpdateSettings(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings) override;

    glm::uvec3 resolution;
    std::shared_ptr<OGLTexture3D> cascadeZero;
    std::array<VolumetricFogCascadeTextures, FOG_CASCADE_COUNT> textures;

    std::shared_ptr<OGLTypedBufferArray<VolumetricFogShape>> fogShapesBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::FogArea>> fogAreaBuffer;
    std::shared_ptr<OGLTypedBuffer<GLSL::FogSettings>> fogSettingsBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::FogCamera>> fogCamerasBuffer;
    std::shared_ptr<OGLTexture3D> noiseTexture;
    std::shared_ptr<OGLSampler> sampler;

private:
    void _UpdateComputePass(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems);
    void _GetCascadePipelines(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems, const uint32_t& a_CascadeIndex);
    OGLFence _executionFence { true };
    OGLCmdBuffer _cmdBuffer;
    std::shared_ptr<OGLProgram> _programParticipating;
    std::shared_ptr<OGLProgram> _programLightsInject;
    std::shared_ptr<OGLProgram> _programIntegration;
    std::shared_ptr<OGLProgram> _programTAA;
};
}