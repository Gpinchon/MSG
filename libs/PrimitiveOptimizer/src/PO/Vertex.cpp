#include <PO/Vertex.hpp>

#include <glm/gtx/hash.hpp>

size_t std::hash<MSG::PO::Vertex>::operator()(const MSG::PO::Vertex& a_Vertex) const
{
    return std::hash<MSG::PO::posType> {}(a_Vertex.position);
}

namespace MSG::PO {
bool Vertex::operator!=(const Vertex& a_Rhs) const
{
    return position != a_Rhs.position;
}
bool Vertex::operator==(const Vertex& a_Rhs) const { return !(*this != a_Rhs); }
}
