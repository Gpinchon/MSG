#include <Renderer/OGL/RAII/IndexBuffer.hpp>
#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RAII/VertexBuffer.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
auto CreateVertexArray()
{
    unsigned handle = 0;
    glCreateVertexArrays(1, &handle);
    return handle;
}
VertexArray::VertexArray(const VertexBuffer& a_VertexBuffer)
    : handle(CreateVertexArray())
{
    for (auto& attrib : a_VertexBuffer.attributesDescription) {
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
    glVertexArrayVertexBuffer(
        handle,
        0, // bindingIndex
        a_VertexBuffer.handle,
        0, // offset
        a_VertexBuffer.stride);
}

VertexArray::VertexArray(const VertexBuffer& a_VertexBuffer, const IndexBuffer& a_IndexBuffer)
    : VertexArray(a_VertexBuffer)
{
    glVertexArrayElementBuffer(handle, a_IndexBuffer);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &handle);
}
}
