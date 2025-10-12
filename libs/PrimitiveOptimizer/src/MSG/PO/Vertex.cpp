#include <MSG/PO/Vertex.hpp>

#include <glm/gtx/hash.hpp>

size_t std::hash<Msg::PO::Vertex>::operator()(const Msg::PO::Vertex& a_Vertex) const
{
    return std::hash<Msg::PO::posType> {}(a_Vertex.position);
}

namespace Msg::PO {
bool Vertex::operator!=(const Vertex& a_Rhs) const
{
    return position != a_Rhs.position;
}
bool Vertex::operator==(const Vertex& a_Rhs) const { return !(*this != a_Rhs); }
}
