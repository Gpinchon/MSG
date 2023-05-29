#pragma once

#include <OCRA/Handle.hpp>
#include <Renderer/OCRA/VertexBuffer.hpp>

OCRA_DECLARE_HANDLE(OCRA::Buffer);
OCRA_DECLARE_HANDLE(OCRA::Shader::Stage);

namespace TabGraph::SG {
class Primitive;
}

namespace TabGraph::Renderer {
struct Primitive {
    Primitive(
        const OCRA::PhysicalDevice::Handle& a_PhysicalDevice,
        const OCRA::Device::Handle& a_Device,
        const SG::Primitive& a_Primitive);
    VertexBuffer vertexBuffer;
    size_t indicesCount { 0 };
    OCRA::Buffer::Handle indices;
    OCRA::Shader::Stage::Handle vertexShader;
};
} // namespace TabGraph::Renderer
