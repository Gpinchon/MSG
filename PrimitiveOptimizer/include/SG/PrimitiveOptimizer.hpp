#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/BoundingVolume.hpp>

#include <SG/POBiMap.hpp>
#include <SG/POPair.hpp>
#include <SG/POReference.hpp>
#include <SG/POTriangle.hpp>
#include <SG/POVertex.hpp>

#include <sparsehash/sparse_hash_map>

#include <array>
#include <memory>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Primitive;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class PrimitiveOptimizer {
public:
    PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive);
    std::shared_ptr<Primitive> operator()(const float& a_CompressionRatio = 0.5f, const float& a_MaxCompressionCost = std::numeric_limits<float>::infinity());
    std::shared_ptr<Primitive> result;

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
    google::sparse_hash_map<uint64_t, POReference> _references;
    POBiMap<POVertex> _vertice;
    POBiMap<POTriangle> _triangles;
    POBiMap<POPair> _pairs;

    google::sparse_hash_map<uint64_t, uint64_t> _pairRefCounts;
    std::vector<uint64_t> _pairIndice;

    template <typename Accessor>
    void _FromIndexed(const std::shared_ptr<Primitive>& a_Primitive, const Accessor& a_Indice);
    void _PushTriangle(const std::shared_ptr<Primitive>& a_Primitive, const std::array<uint32_t, 3>& a_Indice);

    uint64_t _Triangle_Insert(const POTriangle& a_Triangle);
    void _Triangle_Delete(const uint64_t& a_TriangleI);
    ///@return false is triangle is collapsed
    bool _Triangle_Update(const uint64_t& a_TriangleI);
    bool _Triangle_Update(const POTriangle& a_Triangle) const;
    bool _Triangle_IsCollapsed(const uint64_t& a_Triangle) const;
    bool _Triangle_IsCollapsed(const POTriangle& a_Triangle) const;
    void _Triangle_HandleInversion(POTriangle& a_Triangle) const;
    void _Triangle_UpdateVertice(const uint64_t& a_TriangleI);
    void _Triangle_UpdateVertice(const POTriangle& a_Triangle);

    uint64_t _Vertex_Insert(const POVertex& a_V);
    uint64_t _Vertex_Insert_Merge(const POVertex& a_V);
    void _Vertex_Delete(const uint64_t& a_I);
    POVertex _Vertex_Merge(const uint64_t& a_I0, const uint64_t& a_I1, const float& a_X = 0.5f);
    POVertex _Vertex_Merge(const POVertex& a_V0, const POVertex& a_V1, const float& a_X = 0.5f);

    uint64_t _Pair_Ref(const uint64_t& a_VertexI0, const uint64_t& a_VertexI1);
    uint64_t _Pair_Ref(const POPair& a_Pair);
    void _Pair_Unref(const uint64_t& a_PairI);
    void _Pair_Unref(const POPair& a_Pair);
    void _Pair_Update(const uint64_t& a_PairI);
    void _Pair_Update(const POPair& a_Pair);
    void _Pair_Sort();

    void _Preserve_Bounds(const uint64_t& a_TriangleI);
    void _Preserve_Bounds(const POTriangle& a_Triangle);
    void _Cleanup();
    bool _CheckReferencesValidity() const;

    std::shared_ptr<Primitive> _ReconstructPrimitive() const;
};
}
