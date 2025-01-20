#include <SG/POVertex.hpp>

#include <glm/gtx/hash.hpp>

size_t std::hash<MSG::SG::POVertex>::operator()(const MSG::SG::POVertex& a_Vertex) const
{
    return std::hash<MSG::SG::posType> {}(a_Vertex.position);
}

namespace MSG::SG {
bool POVertex::operator!=(const POVertex& a_Rhs) const
{
    return position != a_Rhs.position;
}
bool POVertex::operator==(const POVertex& a_Rhs) const { return !(*this != a_Rhs); }
}
