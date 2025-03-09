#pragma once
#include <MSG/OGLRenderPass.hpp>
#include <MSG/Renderer/OGL/LightCullerFwd.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>

#include <Camera.glsl>
#include <FrameInfo.glsl>

#include <vector>

namespace MSG {
class OGLContext;
class OGLFrameBuffer;
class OGLSampler;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer {
class PathFwd : public Path {
public:
    explicit PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings);
    ~PathFwd() override = default;
    void Update(Renderer::Impl& a_Renderer) override;

private:
    std::shared_ptr<OGLRenderPass> _CreateRenderPass(const OGLRenderPassInfo& a_Info);
    OGLBindings _GetGlobalBindings() const;
    void _UpdateFrameInfo(Renderer::Impl& a_Renderer);
    void _UpdateCamera(Renderer::Impl& a_Renderer);
    void _UpdateLights(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassShadows(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassOpaque(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassBlended(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassCompositing(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);

    Tools::FixedSizeMemoryPool<OGLRenderPass, 1024> _renderPassMemoryPool;
    LightCullerFwd _lightCuller;
    std::shared_ptr<OGLTypedBuffer<GLSL::FrameInfo>> _frameInfoBuffer;
    std::shared_ptr<OGLTypedBuffer<GLSL::CameraUBO>> _cameraBuffer;
    std::shared_ptr<OGLSampler> _shadowSampler;
    std::shared_ptr<OGLSampler> _TAASampler;
    std::shared_ptr<OGLSampler> _iblSpecSampler;
    std::shared_ptr<OGLSampler> _brdfLutSampler;
    std::shared_ptr<OGLTexture> _brdfLut;
    OGLShaderState _shaderShadowMetRough;
    OGLShaderState _shaderShadowSpecGloss;
    OGLShaderState _shaderShadowMetRoughCube;
    OGLShaderState _shaderShadowSpecGlossCube;
    OGLShaderState _shaderMetRoughOpaque;
    OGLShaderState _shaderSpecGlossOpaque;
    OGLShaderState _shaderMetRoughBlended;
    OGLShaderState _shaderSpecGlossBlended;
    OGLShaderState _shaderMetRoughOpaqueUnlit;
    OGLShaderState _shaderSpecGlossOpaqueUnlit;
    OGLShaderState _shaderMetRoughBlendedUnlit;
    OGLShaderState _shaderSpecGlossBlendedUnlit;
    OGLShaderState _shaderCompositing;
    OGLShaderState _shaderTemporalAccumulation;
    OGLShaderState _shaderBloom;
    OGLShaderState _shaderPresent;
    std::shared_ptr<OGLVertexArray> _presentVAO;
    std::shared_ptr<OGLFrameBuffer> _fbOpaque;
    std::shared_ptr<OGLFrameBuffer> _fbBlended;
    std::shared_ptr<OGLFrameBuffer> _fbCompositing;
    std::shared_ptr<OGLFrameBuffer> _fbTemporalAccumulation[2];
    std::shared_ptr<OGLFrameBuffer> _fbPresent;
    std::weak_ptr<OGLRenderPass> _renderPassOpaque;
    std::weak_ptr<OGLRenderPass> _renderPassBlended;
    std::weak_ptr<OGLRenderPass> _renderPassCompositing;
    std::weak_ptr<OGLRenderPass> _renderPassTemporalAccumulation;
    std::weak_ptr<OGLRenderPass> _renderPassPresent;
};
}
