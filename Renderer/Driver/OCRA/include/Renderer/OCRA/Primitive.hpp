#pragma once

#include <OCRA/Handle.hpp>

#include <Renderer/Handles.hpp>
#include <Renderer/OCRA/VertexBuffer.hpp>
#include <Renderer/OCRA/AttributeBuffer.hpp>

namespace TabGraph::SG {
class Primitive;
}

namespace TabGraph::Renderer {
struct Primitive {
    Primitive(
        const Renderer::Impl& a_Renderer,
        const SG::Primitive& a_Primitive);
    OCRA::PrimitiveTopology topology;
    OCRA::PipelineShaderStage vertexShader;
    VertexBuffer vertexBuffer;
    AttributeBuffer indexBuffer;
    std::vector<OCRA::DescriptorSetBinding> bindings;
};
} // namespace TabGraph::Renderer
