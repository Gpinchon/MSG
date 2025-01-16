#include <SG/POTriangle.hpp>
#include <Tools/ArrayHasher.hpp>

size_t std::hash<TabGraph::SG::POTriangle>::operator()(const TabGraph::SG::POTriangle& a_Triangle) const
{
    return TabGraph::Tools::HashArray(a_Triangle.vertice);
}

namespace TabGraph::SG {
POVertexAttribs POTriangle::InterpolateAttribs(const glm::vec3& a_BarycentricCoord) const
{
    POVertexAttribs out;
    out.normal    = norType(0);
    out.tangent   = tanType(0);
    out.texCoord0 = texType(0);
    out.texCoord1 = texType(0);
    out.texCoord2 = texType(0);
    out.texCoord3 = texType(0);
    out.color     = colType(0);
    for (uint8_t i = 0; i < 3; i++) {
        auto& attrib = attribs[i];
        out.normal += attrib.normal * a_BarycentricCoord[i];
        out.tangent += attrib.tangent * a_BarycentricCoord[i];
        out.texCoord0 += attrib.texCoord0 * a_BarycentricCoord[i];
        out.texCoord1 += attrib.texCoord1 * a_BarycentricCoord[i];
        out.texCoord2 += attrib.texCoord2 * a_BarycentricCoord[i];
        out.texCoord3 += attrib.texCoord3 * a_BarycentricCoord[i];
        out.color += attrib.color * a_BarycentricCoord[i];
    }
    return out;
}
bool POTriangle::operator==(const POTriangle& a_Lhs) const { return vertice == a_Lhs.vertice; }
}
