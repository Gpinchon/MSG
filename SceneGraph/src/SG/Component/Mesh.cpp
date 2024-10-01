#include <SG/Component/Mesh.hpp>
#include <SG/Core/Primitive.hpp>

namespace TabGraph::SG::Component {
void Mesh::ComputeBoundingVolume()
{
    boundingVolume = {};
    for (auto& primitive : primitives) {
        boundingVolume += primitive.first->GetBoundingVolume();
    }
}
}
