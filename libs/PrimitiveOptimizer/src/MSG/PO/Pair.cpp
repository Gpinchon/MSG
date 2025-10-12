#include <MSG/PO/Pair.hpp>
#include <MSG/Tools/ArrayHasher.hpp>

size_t std::hash<Msg::PO::Pair>::operator()(const Msg::PO::Pair& a_Pair) const
{
    return Msg::Tools::HashArray(a_Pair.vertice);
}

namespace Msg::PO {
Pair::Pair(const uint64_t& a_V0, const uint64_t& a_V1)
    : vertice({ std::min(a_V0, a_V1), std::max(a_V0, a_V1) })
{
}
bool Pair::operator==(const PO::Pair& a_Lhs) const { return vertice == a_Lhs.vertice; }
}
