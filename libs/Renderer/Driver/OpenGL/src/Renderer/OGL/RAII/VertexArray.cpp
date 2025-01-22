#include <Buffer/Accessor.hpp>
#include <Mesh/Primitive.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>

#include <GL/glew.h>

namespace MSG::Renderer::RAII {
auto CreateVertexArray()
{
    unsigned handle = 0;
    glCreateVertexArrays(1, &handle);
    return handle;
}

VertexArray::VertexArray(
    const unsigned a_VertexCount,
    const std::vector<VertexAttributeDescription>& a_AttributesDesc,
    const std::vector<VertexBindingDescription>& a_VertexBindings)
    : handle(CreateVertexArray())
    , vertexCount(a_VertexCount)
    , attributesDesc(a_AttributesDesc)
    , vertexBindings(a_VertexBindings)
{
    for (auto& attrib : attributesDesc) {
        glEnableVertexArrayAttrib(
            handle,
            attrib.location);
        glVertexArrayAttribBinding(
            handle,
            attrib.location,
            attrib.binding);
        glVertexArrayAttribFormat(
            handle,
            attrib.location,
            attrib.format.size,
            attrib.format.type,
            attrib.format.normalized,
            attrib.offset);
    }
    for (auto& binding : vertexBindings) {
        glVertexArrayVertexBuffer(
            handle,
            binding.index,
            *binding.buffer,
            binding.offset,
            binding.stride);
    }
}

VertexArray::VertexArray(
    const unsigned a_VertexCount,
    const std::vector<VertexAttributeDescription>& a_AttributesDesc,
    const std::vector<VertexBindingDescription>& a_VertexBindings,
    const unsigned a_IndexCount,
    const IndexDescription& a_IndexDesc,
    const std::shared_ptr<Buffer>& a_IndexBuffer)
    : VertexArray(a_VertexCount, a_AttributesDesc, a_VertexBindings)
{
    indexed     = true;
    indexCount  = a_IndexCount;
    indexDesc   = a_IndexDesc;
    indexBuffer = a_IndexBuffer;
    glVertexArrayElementBuffer(handle, *indexBuffer);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &handle);
}
}
