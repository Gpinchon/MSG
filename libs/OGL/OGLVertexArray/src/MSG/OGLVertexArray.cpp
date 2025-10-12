#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLVertexArray.hpp>

#include <GL/glew.h>

namespace Msg {
auto CreateVertexArray(OGLContext& a_Context)
{
    unsigned handle = 0;
    ExecuteOGLCommand(a_Context, [&handle] { glCreateVertexArrays(1, &handle); }, true);
    return handle;
}

OGLVertexArray::OGLVertexArray(
    OGLContext& a_Context,
    const unsigned a_VertexCount,
    const std::vector<OGLVertexAttributeDescription>& a_AttributesDesc,
    const std::vector<OGLVertexBindingDescription>& a_VertexBindings)
    : handle(CreateVertexArray(a_Context))
    , vertexCount(a_VertexCount)
    , attributesDesc(a_AttributesDesc)
    , vertexBindings(a_VertexBindings)
    , context(a_Context)
{
    ExecuteOGLCommand(context, [handle = handle, attributesDesc = attributesDesc, vertexBindings = vertexBindings] {
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
    });
}

OGLVertexArray::OGLVertexArray(
    OGLContext& a_Context,
    const unsigned a_VertexCount,
    const std::vector<OGLVertexAttributeDescription>& a_AttributesDesc,
    const std::vector<OGLVertexBindingDescription>& a_VertexBindings,
    const unsigned a_IndexCount,
    const OGLIndexDescription& a_IndexDesc,
    const std::shared_ptr<OGLBuffer>& a_IndexBuffer)
    : OGLVertexArray(a_Context, a_VertexCount, a_AttributesDesc, a_VertexBindings)
{
    indexed     = true;
    indexCount  = a_IndexCount;
    indexDesc   = a_IndexDesc;
    indexBuffer = a_IndexBuffer;
    ExecuteOGLCommand(context, [handle = handle, indexBuffer = indexBuffer] { glVertexArrayElementBuffer(handle, *indexBuffer); });
}

OGLVertexArray::~OGLVertexArray()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteVertexArrays(1, &handle); });
}
}