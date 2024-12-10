#include <SG/Component/Mesh.hpp>
#include <SG/Core/Primitive.hpp>

namespace TabGraph::SG::Component {
Mesh::Mesh()
    : MeshLods(1)
{
}

Mesh::Mesh(const std::string& a_Name)
    : Mesh()
{
    name = a_Name;
}
void Mesh::ComputeBoundingVolume()
{
    boundingVolume = {};
    for (auto& [primitive, material] : front()) {
        boundingVolume += primitive->GetBoundingVolume();
    }
}
}
