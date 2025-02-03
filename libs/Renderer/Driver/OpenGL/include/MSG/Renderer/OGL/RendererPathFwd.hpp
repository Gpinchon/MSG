#pragma once
#include <MSG/OGLRenderPass.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>
#include <MSG/Renderer/OGL/UniformBufferUpdate.hpp>

#include <Camera.glsl>

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
    void _UpdateRenderPassShadows(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassOpaque(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassBlended(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassCompositing(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);
    Tools::FixedSizeMemoryPool<OGLRenderPass, 1024> _renderPassMemoryPool;
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
    std::shared_ptr<OGLSampler> _clampedSampler;
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
