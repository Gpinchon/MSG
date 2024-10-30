#include <SG/POTriangle.hpp>

#include <Tools/HashCombine.hpp>

size_t std::hash<TabGraph::SG::POTriangle>::operator()(const TabGraph::SG::POTriangle& a_Triangle) const
{
    std::size_t seed = 0;
    TABGRAPH_HASH_COMBINE(seed, a_Triangle.vertice[0]);
    TABGRAPH_HASH_COMBINE(seed, a_Triangle.vertice[1]);
    TABGRAPH_HASH_COMBINE(seed, a_Triangle.vertice[2]);
    return seed;
}

namespace TabGraph::SG {
bool POTriangle::operator==(const POTriangle& a_Lhs) const { return vertice == a_Lhs.vertice; }
}
