#pragma once
#include <Renderer/OGL/RenderPass.hpp>
#include <Renderer/OGL/RendererPath.hpp>
#include <Renderer/OGL/UniformBufferUpdate.hpp>

#include <Camera.glsl>

#include <vector>

namespace TabGraph::Renderer {
class Impl;
class Context;
}

namespace TabGraph::Renderer {
class PathFwd : public Path {
public:
    explicit PathFwd(Renderer::Impl& a_Renderer, const RendererSettings& a_Settings);
    ~PathFwd() override = default;
    void Update(Renderer::Impl& a_Renderer) override;
    void Execute() override;

private:
    std::shared_ptr<RenderPass> _CreateRenderPass(const RenderPassInfo& a_Info);
    void _UpdateRenderPassOpaque(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassBlended(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassCompositing(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassTemporalAccumulation(Renderer::Impl& a_Renderer);
    void _UpdateRenderPassPresent(Renderer::Impl& a_Renderer);
    Tools::FixedSizeMemoryPool<RenderPass, 32> _renderPassMemoryPool;
    ShaderState _shaderMetRoughOpaque;
    ShaderState _shaderSpecGlossOpaque;
    ShaderState _shaderMetRoughBlended;
    ShaderState _shaderSpecGlossBlended;
    ShaderState _shaderCompositing;
    ShaderState _shaderTemporalAccumulation;
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