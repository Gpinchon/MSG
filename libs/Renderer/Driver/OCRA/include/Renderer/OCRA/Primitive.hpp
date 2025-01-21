#pragma once

#include <OCRA/Handle.hpp>

#include <Renderer/Handles.hpp>
#include <Renderer/OCRA/AttributeBuffer.hpp>
#include <Renderer/OCRA/VertexBuffer.hpp>

namespace MSG::SG {
class Primitive;
}

namespace MSG::Renderer {
struct Primitive {
    Primitive(
        const Renderer::Impl& a_Renderer,
        const Core::Primitive& a_Primitive);
    OCRA::PrimitiveTopology topology;
    VertexBuffer vertexBuffer;
    AttributeBuffer indexBuffer;
};
} // namespace MSG::Renderer
