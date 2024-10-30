#include <SG/POVertex.hpp>

#include <Tools/HashCombine.hpp>

#include <glm/gtx/hash.hpp>

size_t std::hash<TabGraph::SG::POVertex>::operator()(const TabGraph::SG::POVertex& a_Vertex) const
{
    std::size_t seed = 0;
    TABGRAPH_HASH_COMBINE(seed, a_Vertex.position);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.normal);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.tangent);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord0);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord1);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord2);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord3);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.color);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.joints);
    // TABGRAPH_HASH_COMBINE(seed, a_Vertex.weights);
    return seed;
}

namespace TabGraph::SG {
bool POVertex::operator!=(const POVertex& a_Rhs) const
{
    return position != a_Rhs.position;
    //|| normal != a_Rhs.normal
    //|| tangent != a_Rhs.tangent
    //|| texCoord0 != a_Rhs.texCoord0
    //|| texCoord1 != a_Rhs.texCoord1
    //|| texCoord2 != a_Rhs.texCoord2
    //|| texCoord3 != a_Rhs.texCoord3
    //|| color != a_Rhs.color
    //|| joints != a_Rhs.joints
    //|| weights != a_Rhs.weights;
}
bool POVertex::operator==(const POVertex& a_Rhs) const { return !(*this != a_Rhs); }
}
