#include <MSG/Mesh.hpp>
#include <MSG/Mesh/Primitive.hpp>

namespace Msg {
void Mesh::ComputeBoundingVolume()
{
    boundingVolume = {};
    for (auto& [primitive, material] : front()) {
        boundingVolume += primitive->GetBoundingVolume();
    }
}
}
