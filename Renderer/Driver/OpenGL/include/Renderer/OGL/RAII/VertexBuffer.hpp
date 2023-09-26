#pragma once
#include <Renderer/OGL/RAII/Buffer.hpp>

#include <cstddef>
#include <vector>

namespace TabGraph::SG {
class Primitive;
}

namespace TabGraph::Renderer::RAII {
struct VertexAttribFormat {
    unsigned size; // the number of component per vertex
    unsigned type; // the type of data (float, int...)
    bool normalized;
};
struct VertexAttributeDescription {
    VertexAttribFormat format;
    unsigned offset; //(Relative offset) the distance between elements in the buffer
    unsigned binding; // The binding number this attribute takes its data from
    unsigned location; // Location in the shader for this attribute
};

struct VertexBuffer : Buffer {
    VertexBuffer(const SG::Primitive& a_Primitive);
    ~VertexBuffer();
    unsigned stride;
    unsigned vertexCount;
    std::vector<VertexAttributeDescription> attributesDescription;
};
}
