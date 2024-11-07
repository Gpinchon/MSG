#include <SG/POVertex.hpp>

#include <glm/gtx/hash.hpp>

size_t std::hash<TabGraph::SG::POVertex>::operator()(const TabGraph::SG::POVertex& a_Vertex) const
{
    return std::hash<TabGraph::SG::posType> {}(a_Vertex.position);
}

namespace TabGraph::SG {
bool POVertex::operator!=(const POVertex& a_Rhs) const
{
    return position != a_Rhs.position;
}
bool POVertex::operator==(const POVertex& a_Rhs) const { return !(*this != a_Rhs); }
}
