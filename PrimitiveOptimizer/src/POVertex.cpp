#include <SG/POVertex.hpp>

#include <Tools/HashCombine.hpp>

#include <glm/gtx/hash.hpp>

size_t std::hash<TabGraph::SG::POVertex>::operator()(const TabGraph::SG::POVertex& a_Vertex) const
{
    std::size_t seed = 0;
    TABGRAPH_HASH_COMBINE(seed, a_Vertex.position);
    return seed;
}

namespace TabGraph::SG {
bool POVertex::operator!=(const POVertex& a_Rhs) const
{
    return position != a_Rhs.position;
}
bool POVertex::operator==(const POVertex& a_Rhs) const { return !(*this != a_Rhs); }
}
