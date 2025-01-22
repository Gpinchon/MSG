#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/PO/BiMap.hpp>
#include <MSG/PO/Pair.hpp>
#include <MSG/PO/Reference.hpp>
#include <MSG/PO/Triangle.hpp>
#include <MSG/PO/Vertex.hpp>

#include <sparsehash/sparse_hash_map>

#include <array>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Core {
class Primitive;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class PrimitiveOptimizer {
public:
    PrimitiveOptimizer(const std::shared_ptr<MeshPrimitive>& a_Primitive);
    /**
     * @param a_MaxCompressionCost : the maximum compression cost to check for
     * @return true if there are pairs that can be collapsed
     */
    bool CanCompress(const float& a_MaxCompressionCost) const { return !_pairIndice.empty() && _pairs.at(_pairIndice.back()).contractionCost < a_MaxCompressionCost; }
    std::shared_ptr<MeshPrimitive> operator()(const float& a_CompressionRatio = 0.5f, const float& a_MaxCompressionCost = std::numeric_limits<float>::infinity());
    std::shared_ptr<MeshPrimitive> result;
    float resultCompressionRatio = 0.f;

private:
    glm::vec3 _min, _max;
    const bool _hasNormals;
    const bool _hasTangents;
    const bool _hasTexCoord0;
    const bool _hasTexCoord1;
    const bool _hasTexCoord2;
    const bool _hasTexCoord3;
    const bool _hasColors;
    const bool _hasJoints;
    const bool _hasWeights;
    google::sparse_hash_map<uint64_t, PO::Reference> _references;
    PO::BiMap<PO::Vertex> _vertice;
    PO::BiMap<PO::Triangle> _triangles;
    PO::BiMap<PO::Pair> _pairs;

    google::sparse_hash_map<uint64_t, uint64_t> _pairRefCounts;
    std::vector<uint64_t> _pairIndice;

    template <typename Accessor>
    void _FromIndexed(const std::shared_ptr<MeshPrimitive>& a_Primitive, const Accessor& a_Indice);
    void _PushTriangle(const std::shared_ptr<MeshPrimitive>& a_Primitive, const std::array<uint32_t, 3>& a_Indice);

    uint64_t _Triangle_Insert(const PO::Triangle& a_Triangle);
    void _Triangle_Delete(const uint64_t& a_TriangleI);
    ///@return false is triangle is collapsed
    bool _Triangle_Update(const uint64_t& a_TriangleI);
    bool _Triangle_Update(const PO::Triangle& a_Triangle) const;
    bool _Triangle_IsCollapsed(const uint64_t& a_Triangle) const;
    bool _Triangle_IsCollapsed(const PO::Triangle& a_Triangle) const;
    void _Triangle_HandleInversion(PO::Triangle& a_Triangle) const;
    void _Triangle_UpdateVertice(const uint64_t& a_TriangleI);
    void _Triangle_UpdateVertice(const PO::Triangle& a_Triangle);

    uint64_t _Vertex_Insert(const PO::Vertex& a_V);
    void _Vertex_Delete(const uint64_t& a_I);
    PO::Vertex _Vertex_Merge(const uint64_t& a_I0, const uint64_t& a_I1, const float& a_X = 0.5f);
    PO::Vertex _Vertex_Merge(const PO::Vertex& a_V0, const PO::Vertex& a_V1, const float& a_X = 0.5f);

    uint64_t _Pair_Ref(const uint64_t& a_VertexI0, const uint64_t& a_VertexI1);
    uint64_t _Pair_Ref(const PO::Pair& a_Pair);
    void _Pair_Unref(const uint64_t& a_PairI);
    void _Pair_Unref(const PO::Pair& a_Pair);
    void _Pair_Update(const uint64_t& a_PairI);
    void _Pair_Update(const PO::Pair& a_Pair);
    void _Pair_Sort();

    void _Preserve_Bounds(const uint64_t& a_TriangleI);
    void _Preserve_Bounds(const PO::Triangle& a_Triangle);
    void _Cleanup();
    bool _CheckReferencesValidity() const;

    std::shared_ptr<MeshPrimitive> _ReconstructPrimitive() const;
};
}
