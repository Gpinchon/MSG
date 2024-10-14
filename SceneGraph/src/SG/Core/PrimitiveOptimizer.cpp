#include <SG/Core/PrimitiveOptimizer.hpp>
#include <SG/Core/Primitive.hpp>
#include <Tools/Debug.hpp>

namespace TabGraph::SG {
Vertex GetVertex(const std::shared_ptr<SG::Primitive>& a_Primitive, const uint64_t& a_Index) 
{
    Vertex vertex;
    vertex.position = a_Index;
    return vertex;
}

PrimitiveOptimizer::PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive)
    : primitive(a_Primitive)
{
    if (primitive->GetDrawingMode() != SG::Primitive::DrawingMode::Triangles) {
        errorLog("Mesh optimization only available for triangulated meshes");
        return;
    }
    if (!primitive->GetIndices().empty()) {
        if (primitive->GetIndices().GetComponentType() == SG::DataType::Uint32) {
            TypedBufferAccessor<uint32_t> indice = primitive->GetIndices();
            for (uint32_t i = 0; i < primitive->GetIndices().GetSize(); i += 3) {}
                //functor(indice.at(i + 0), indice.at(i + 1), indice.at(i + 2));
        } else if (primitive->GetIndices().GetComponentType() == SG::DataType::Uint16) {
            TypedBufferAccessor<uint16_t> indice = primitive->GetIndices();
            for (uint32_t i = 0; i < primitive->GetIndices().GetSize(); i += 3) {}
                //functor(indice.at(i + 0), indice.at(i + 1), indice.at(i + 2));
        }
    }
}
}