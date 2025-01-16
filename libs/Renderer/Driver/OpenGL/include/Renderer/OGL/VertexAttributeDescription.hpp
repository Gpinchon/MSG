#pragma once

/**
 * @brief this header regroups the struct used to describe the vertex attributes
*/

namespace TabGraph::Renderer {
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
}