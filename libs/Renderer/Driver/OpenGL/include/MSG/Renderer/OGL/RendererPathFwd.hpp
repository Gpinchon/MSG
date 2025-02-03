#pragma once
#include <MSG/Renderer/OGL/RenderPass.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>
#include <MSG/Renderer/OGL/UniformBufferUpdate.hpp>

#include <Camera.glsl>

#include <vector>

namespace MSG {
class OGLContext;
class OGLFrameBuffer;
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
    std::shared_ptr<RenderPass> _CreateRenderPass(const RenderPassInfo& a_Info);
    void _UpdateRenderPassShadows(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassOpaque(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassBlended(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassCompositing(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);
    Tools::FixedSizeMemoryPool<RenderPass, 1024> _renderPassMemoryPool;
    ShaderState _shaderMetRoughOpaque;
    ShaderState _shaderSpecGlossOpaque;
    ShaderState _shaderMetRoughBlended;
    ShaderState _shaderSpecGlossBlended;
    ShaderState _shaderMetRoughOpaqueUnlit;
    ShaderState _shaderSpecGlossOpaqueUnlit;
    ShaderState _shaderMetRoughBlendedUnlit;
    ShaderState _shaderSpecGlossBlendedUnlit;
    ShaderState _shaderCompositing;
    ShaderState _shaderTemporalAccumulation;
    ShaderState _shaderBloom;
    ShaderState _shaderPresent;
    std::shared_ptr<OGLVertexArray> _presentVAO;
    std::shared_ptr<OGLFrameBuffer> _fbOpaque;
    std::shared_ptr<OGLFrameBuffer> _fbBlended;
    std::shared_ptr<OGLFrameBuffer> _fbCompositing;
    std::shared_ptr<OGLFrameBuffer> _fbTemporalAccumulation[2];
    std::shared_ptr<OGLFrameBuffer> _fbPresent;
    std::weak_ptr<RenderPass> _renderPassOpaque;
    std::weak_ptr<RenderPass> _renderPassBlended;
    std::weak_ptr<RenderPass> _renderPassCompositing;
    std::weak_ptr<RenderPass> _renderPassTemporalAccumulation;
    std::weak_ptr<RenderPass> _renderPassPresent;
};
}
