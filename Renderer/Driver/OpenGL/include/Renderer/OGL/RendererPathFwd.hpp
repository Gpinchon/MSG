#pragma once
#include <Renderer/OGL/RenderPass.hpp>
#include <Renderer/OGL/RendererPath.hpp>
#include <Renderer/OGL/UniformBufferUpdate.hpp>

#include <Camera.glsl>

#include <vector>

namespace TabGraph::Renderer {
struct Impl;
struct Context;
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
    void _UpdateGraphicsPipelines(Renderer::Impl& a_Renderer, std::vector<GraphicsPipelineInfo>& a_GraphicsPipelines);
    ShaderState _metRoughShader;
    ShaderState _specGlossShader;
    std::shared_ptr<RAII::FrameBuffer> _fb;
    Tools::FixedSizeMemoryPool<RenderPass, 32> renderPassMemoryPool;
    std::shared_ptr<RenderPass> _renderPass;
    std::shared_ptr<RenderPass> _renderPassPresent;
};
}