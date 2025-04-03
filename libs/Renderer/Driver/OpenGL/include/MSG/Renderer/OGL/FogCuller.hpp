#pragma once

#include <Camera.glsl>
#include <Fog.glsl>
#include <MSG/Image.hpp>
#include <MSG/OGLTypedBuffer.hpp>

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
class FogCuller {
public:
    FogCuller(Renderer::Impl& a_Renderer);
    void Update(const Scene& a_Scene);
    OGLRenderPass* GetComputePass(
        const LightCullerFwd& a_LightCuller,
        const std::shared_ptr<OGLSampler>& a_ShadowSampler,
        const std::shared_ptr<OGLBuffer>& a_FrameInfoBuffer);
    OGLContext& context;
    Image participatingMediaImage0;
    Image participatingMediaImage1;

    std::shared_ptr<OGLTypedBuffer<GLSL::FogSettings>> fogSettingsBuffer;
    std::shared_ptr<OGLTypedBuffer<GLSL::CameraUBO>> fogCameraBuffer;
    std::shared_ptr<OGLSampler> noiseSampler;
    std::shared_ptr<OGLTexture3D> noiseTexture;

    /// @brief RGB: Scattering, A: Extinction
    std::shared_ptr<OGLTexture3D> participatingMediaTexture0;
    /// @brief RGB: Emissive, A: Phase(g)
    std::shared_ptr<OGLTexture3D> participatingMediaTexture1;
    /// @brief RGB: Scattered light to camera, A: Extinction
    std::shared_ptr<OGLTexture3D> scatteringTexture;

    std::shared_ptr<OGLTexture3D> resultTexture_Previous;
    std::shared_ptr<OGLTexture3D> resultTexture;

    std::shared_ptr<OGLProgram> lightInjectionProgram;
    std::shared_ptr<OGLProgram> integrationProgram;
    OGLRenderPassInfo renderPassInfo;
};
}