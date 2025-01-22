#pragma once

#include <OCRA/Handle.hpp>

#include <MSG/Renderer/Handles.hpp>
#include <MSG/Renderer/OCRA/AttributeBuffer.hpp>
#include <MSG/Renderer/OCRA/VertexBuffer.hpp>

namespace MSG::SG {
class Primitive;
}

namespace MSG::Renderer {
struct Primitive {
    Primitive(
        const Renderer::Impl& a_Renderer,
        const MeshPrimitive& a_Primitive);
    OCRA::PrimitiveTopology topology;
    VertexBuffer vertexBuffer;
    AttributeBuffer indexBuffer;
};
} // namespace MSG::Renderer
