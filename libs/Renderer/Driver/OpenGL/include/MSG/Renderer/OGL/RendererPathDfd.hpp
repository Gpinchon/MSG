#pragma once
#include <MSG/OGLRenderPass.hpp>
#include <MSG/Renderer/OGL/LightCullerFwd.hpp>
#include <MSG/Renderer/OGL/ObjectRepertory.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>
#include <MSG/Renderer/OGL/VolumetricFog.hpp>

#include <Camera.glsl>
#include <FrameInfo.glsl>
#include <SSAO.glsl>

#include <vector>

namespace MSG {
class OGLContext;
class OGLFrameBuffer;
class OGLSampler;
}

namespace MSG::Renderer {
class Impl;
struct RendererSettings;
}

namespace MSG::Renderer::RenderBuffer {
class Impl;
}

namespace MSG::Renderer {
struct MeshInfo {
    OGLGraphicsPipelineInfo pipeline;
    bool isMetRough;
    bool isSpecGloss;
    bool isUnlit;
};
class PathDfd : public Path {
public:
    explicit PathDfd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings);
    ~PathDfd() override = default;
    void Update(Renderer::Impl& a_Renderer) override;
    void UpdateSettings(
        Renderer::Impl& a_Renderer,
        const RendererSettings& a_Settings) override;
    void UpdateRenderBuffers(
        Renderer::Impl& a_Renderer) override;

private:
    OGLBindings _GetGlobalBindings() const;
    void _UpdateFrameInfo(Renderer::Impl& a_Renderer);
    void _UpdateCamera(Renderer::Impl& a_Renderer);
    void _UpdateLights(Renderer::Impl& a_Renderer);
    void _UpdateShadows(Renderer::Impl& a_Renderer);
    void _UpdateFog(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassGeometry(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassLight(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassWBOIT(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassCompositing(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);

    std::vector<MeshInfo> opaqueMeshes;
    std::vector<MeshInfo> blendedMeshes;
    float _internalRes = 0;
    LightCullerFwd _lightCuller;
    VolumetricFog _volumetricFog;
    std::shared_ptr<OGLTypedBuffer<GLSL::FrameInfo>> _frameInfoBuffer;
    std::shared_ptr<OGLTypedBuffer<GLSL::CameraUBO>> _cameraBuffer;
    std::shared_ptr<OGLTypedBuffer<GLSL::SSAOSettings>> _ssaoBuffer;
    std::shared_ptr<OGLSampler> _fogSampler;
    std::shared_ptr<OGLSampler> _shadowSampler;
    std::shared_ptr<OGLSampler> _TAASampler;
    std::shared_ptr<OGLSampler> _iblSpecSampler;
    std::shared_ptr<OGLSampler> _brdfLutSampler;
    std::shared_ptr<OGLTexture> _brdfLut;
    ObjectRepertory<std::shared_ptr<OGLProgram>> _shaders;
    OGLShaderState _shaderCompositing;
    OGLShaderState _shaderTemporalAccumulation;
    OGLShaderState _shaderBloom;
    OGLShaderState _shaderPresent;
    std::shared_ptr<OGLVertexArray> _presentVAO;
    std::shared_ptr<OGLFrameBuffer> _fbGeometry;
    std::shared_ptr<OGLFrameBuffer> _fbLightPass;
    std::shared_ptr<OGLFrameBuffer> _fbTemporalAccumulation[2];
    std::shared_ptr<OGLFrameBuffer> _fbPresent;

    OGLRenderPassInfo _renderPassGeometryInfo;
    OGLRenderPassInfo _renderPassLightInfo;
    OGLRenderPassInfo _renderPassWBOITInfo;
    OGLRenderPassInfo _renderPassWBOITCompositingInfo;
    OGLRenderPassInfo _renderPassFogInfo;
    OGLRenderPassInfo _renderPassTemporalAccumulationInfo;
    OGLRenderPassInfo _renderPassPresentInfo;

    std::shared_ptr<OGLFrameBuffer> _fbWBOIT;
    std::shared_ptr<OGLFrameBuffer> _fbWBOITCompositing;
    std::shared_ptr<OGLTexture3D> _WBOITAccum;
    std::shared_ptr<OGLTexture3D> _WBOITRevealage;
    std::shared_ptr<OGLTexture2D> _WBOITDepth;
};
}
