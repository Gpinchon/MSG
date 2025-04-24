#pragma once

#include <Camera.glsl>
#include <Fog.glsl>
#include <FogArea.glsl>
#include <FogCamera.glsl>
#include <MSG/Image.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/Structs.hpp>

#include <memory>

namespace MSG::Renderer {
class Impl;
class LightCullerFwd;
}

namespace MSG {
class OGLTexture3D;
class OGLContext;
class OGLProgram;
class OGLBuffer;
class OGLSampler;
class OGLRenderPass;
class Scene;
};

namespace MSG::Renderer {
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
    std::shared_ptr<OGLTexture3D> resultTexture_Previous;
    std::shared_ptr<OGLTexture3D> resultTexture;
};

class VolumetricFog {
public:
    VolumetricFog(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer);
    void UpdateSettings(
        Renderer::Impl& a_Renderer,
        const RendererSettings& a_Settings);
    OGLRenderPass* GetComputePass(
        const LightCullerFwd& a_LightCuller,
        const std::shared_ptr<OGLSampler>& a_ShadowSampler,
        const std::shared_ptr<OGLBuffer>& a_FrameInfoBuffer);
    OGLContext& context;
    glm::uvec3 resolution;
    std::shared_ptr<OGLTexture3D> cascadeZero;
    std::array<VolumetricFogCascadeTextures, FOG_CASCADE_COUNT> textures;

    std::shared_ptr<OGLTypedBufferArray<VolumetricFogShape>> fogShapesBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::FogArea>> fogAreaBuffer;
    std::shared_ptr<OGLTypedBuffer<GLSL::FogSettings>> fogSettingsBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::FogCamera>> fogCamerasBuffer;
    std::shared_ptr<OGLTexture3D> noiseTexture;
    std::shared_ptr<OGLSampler> sampler;

    std::shared_ptr<OGLProgram> participatingMediaProgram;
    std::shared_ptr<OGLProgram> lightInjectionProgram;
    std::shared_ptr<OGLProgram> integrationProgram;
    OGLRenderPassInfo renderPassInfo;

private:
    void _GetCascadePipelines(
        const uint32_t& a_CascadeIndex,
        const LightCullerFwd& a_LightCuller,
        const std::shared_ptr<OGLSampler>& a_ShadowSampler,
        const std::shared_ptr<OGLBuffer>& a_FrameInfoBuffer);
};
}