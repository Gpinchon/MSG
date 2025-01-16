#pragma once

#include <OCRA/Handle.hpp>

#include <Renderer/Handles.hpp>
#include <Renderer/OCRA/AttributeBuffer.hpp>
#include <Renderer/OCRA/VertexBuffer.hpp>

namespace TabGraph::SG {
class Primitive;
}

namespace TabGraph::Renderer {
struct Primitive {
    Primitive(
        const Renderer::Impl& a_Renderer,
        const SG::Primitive& a_Primitive);
    OCRA::PrimitiveTopology topology;
    VertexBuffer vertexBuffer;
    AttributeBuffer indexBuffer;
};
} // namespace TabGraph::Renderer
