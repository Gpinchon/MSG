#include <MSG/Debug.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLVertexArray.hpp>
#include <MSG/Renderer/OGL/Primitive.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>
#include <MSG/Renderer/OGL/Vertex.hpp>

#include <GL/glew.h>
#include <stdexcept>

namespace MSG::Renderer {
Primitive::Primitive(OGLContext& a_Context, MeshPrimitive& a_Primitive)
    : drawMode(ToGL(a_Primitive.GetDrawingMode()))
{
    constexpr auto attribsDesc = GetVertexAttributeDescription();
    auto vertice               = a_Primitive.GetVertices();
    auto vertexBuffer          = std::make_shared<OGLBuffer>(a_Context, vertice.size() * sizeof(Vertex), vertice.data(), 0);

    OGLVertexBindingDescription binding;
    binding.buffer = vertexBuffer;
    binding.index  = 0;
    binding.offset = 0;
    binding.stride = sizeof(Vertex);

    std::vector<OGLVertexAttributeDescription> attribs(attribsDesc.begin(), attribsDesc.end());
    std::vector<OGLVertexBindingDescription> bindings { binding };

    auto& indice = a_Primitive.GetIndices();
    if (!indice.empty()) {
        auto indexBuffer              = std::make_shared<OGLBuffer>(a_Context,
                         indice.size() * sizeof(unsigned), indice.data(), 0);
        OGLIndexDescription indexDesc = {};
        indexDesc.type                = GL_UNSIGNED_INT;
        vertexArray                   = std::make_shared<OGLVertexArray>(a_Context,
                              uint32_t(vertice.size()), attribs, bindings,
                              uint32_t(indice.size()), indexDesc, indexBuffer);
    } else {
        vertexArray = std::make_shared<OGLVertexArray>(a_Context,
            uint32_t(vertice.size()), attribs, bindings);
    }
}
}
