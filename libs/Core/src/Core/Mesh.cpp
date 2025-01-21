#include <Core/Mesh.hpp>
#include <Core/Primitive.hpp>

namespace MSG::Core {
void Mesh::ComputeBoundingVolume()
{
    boundingVolume = {};
    for (auto& [primitive, material] : front()) {
        boundingVolume += primitive->GetBoundingVolume();
    }
}
}
