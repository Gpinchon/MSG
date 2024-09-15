#include <SG/Component/Mesh.hpp>
#include <SG/Core/Primitive.hpp>

namespace TabGraph::SG::Component {
void Mesh::ComputeAABB()
{
    aabb         = {};
    float weight = 1.f / primitives.size();
    for (auto& primitive : primitives) {
        auto& primitiveAABB = primitive.first->GetAABB();
        aabb.center += primitiveAABB.center * weight;
        aabb.halfSize = glm::max(aabb.halfSize, primitiveAABB.halfSize);
    }
}
}
