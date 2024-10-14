#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <memory>
#include <array>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Primitive;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
struct Vertex {
    int64_t position  = 0;
    int64_t normal    = 0;
    int64_t tangent   = 0;
    int64_t texCoord0 = 0;
    int64_t texCoord1 = 0;
    int64_t texCoord2 = 0;
    int64_t texCoord3 = 0;
    int64_t joints    = 0;
    int64_t weights   = 0;
};
struct Triangle {
    std::array<int64_t, 3> vertice;
    std::array<int64_t, 3> neighbors;
};

class PrimitiveOptimizer {
    PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive);
    std::shared_ptr<Primitive> operator()(const float& a_Aggressiveness) const;

    std::vector<Triangle> triangles;
    std::vector<Vertex> vertice;
    const std::shared_ptr<Primitive>& primitive;
};
}
