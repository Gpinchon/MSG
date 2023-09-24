#include <Renderer/OGL/RAII/VertexArray.hpp>
#include <Renderer/OGL/RAII/VertexBuffer.hpp>

#include <GL/glew.h>

namespace TabGraph::Renderer::RAII {
VertexArray::VertexArray(const VertexBuffer& a_VertexBuffer)
{
    glCreateVertexArrays(1, &handle);
    for (auto& attrib : a_VertexBuffer.attributesDescription) {
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
        0); // stride
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &handle);
}
}
