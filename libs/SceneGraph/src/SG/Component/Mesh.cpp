#include <SG/Component/Mesh.hpp>
#include <SG/Core/Primitive.hpp>

namespace MSG::SG::Component {
void Mesh::ComputeBoundingVolume()
{
    boundingVolume = {};
    for (auto& [primitive, material] : front()) {
        boundingVolume += primitive->GetBoundingVolume();
    }
}
}
