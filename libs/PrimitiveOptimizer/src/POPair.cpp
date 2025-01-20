#include <SG/POPair.hpp>
#include <Tools/ArrayHasher.hpp>

size_t std::hash<MSG::SG::POPair>::operator()(const MSG::SG::POPair& a_Pair) const
{
    return MSG::Tools::HashArray(a_Pair.vertice);
}

namespace MSG::SG {
POPair::POPair(const uint64_t& a_V0, const uint64_t& a_V1)
    : vertice({ std::min(a_V0, a_V1), std::max(a_V0, a_V1) })
{
}
bool POPair::operator==(const POPair& a_Lhs) const { return vertice == a_Lhs.vertice; }
}
