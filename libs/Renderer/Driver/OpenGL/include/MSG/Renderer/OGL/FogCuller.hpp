#pragma once

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
    OGLRenderPass* Update(
        const Scene& a_Scene,
        const LightCullerFwd& a_LightCuller,
        const std::shared_ptr<OGLSampler>& a_ShadowSampler,
        const std::shared_ptr<OGLBuffer>& a_CameraBuffer,
        const std::shared_ptr<OGLBuffer>& a_FrameInfoBuffer);
    OGLContext& context;
    Image image;
    std::shared_ptr<OGLTypedBuffer<GLSL::FogSettings>> fogSettingsBuffer;
    std::shared_ptr<OGLSampler> noiseSampler;
    std::shared_ptr<OGLTexture3D> noiseTexture;
    std::shared_ptr<OGLTexture3D> densityTexture;
    std::shared_ptr<OGLTexture3D> resultTexture;
    std::shared_ptr<OGLProgram> cullingProgram;
    OGLRenderPassInfo renderPassInfo;
};
}