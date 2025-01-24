#pragma once
#include <MSG/Renderer/OGL/RenderPass.hpp>
#include <MSG/Renderer/OGL/RendererPath.hpp>
#include <MSG/Renderer/OGL/UniformBufferUpdate.hpp>

#include <Camera.glsl>

#include <vector>

namespace MSG {
class OGLContext;
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
    std::shared_ptr<RAII::VertexArray> _presentVAO;
    std::shared_ptr<RAII::FrameBuffer> _fbOpaque;
    std::shared_ptr<RAII::FrameBuffer> _fbBlended;
    std::shared_ptr<RAII::FrameBuffer> _fbCompositing;
    std::shared_ptr<RAII::FrameBuffer> _fbTemporalAccumulation[2];
    std::shared_ptr<RAII::FrameBuffer> _fbPresent;
    std::shared_ptr<RenderPass> _renderPassOpaque;
    std::shared_ptr<RenderPass> _renderPassBlended;
    std::shared_ptr<RenderPass> _renderPassCompositing;
    std::shared_ptr<RenderPass> _renderPassTemporalAccumulation;
    std::shared_ptr<RenderPass> _renderPassPresent;
};
}
