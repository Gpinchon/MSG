#include <Mesh.hpp>
#include <Mesh/Primitive.hpp>

namespace MSG {
void Mesh::ComputeBoundingVolume()
{
    boundingVolume = {};
    for (auto& [primitive, material] : front()) {
        boundingVolume += primitive->GetBoundingVolume();
    }
}
}
