#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/BoundingVolume.hpp>

#include <glm/mat4x4.hpp>

#include <array>
#include <memory>
#include <unordered_map>
#include <unordered_set>
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
    PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive, const float& a_DistanceThreshold = 0.01f);
    std::shared_ptr<Primitive> operator()(const float& a_CompressionRatio = 0.5f, const float& a_MaxCompressionCost = std::numeric_limits<float>::infinity());
    std::shared_ptr<Primitive> result;

private:
    template <typename Type>
    class BiMap {
    public:
        using IndexType = uint64_t;
        using HashType  = uint64_t;
        using MapAType  = std::unordered_map<HashType, IndexType>;
        using MapBType  = std::unordered_map<IndexType, Type>;
        IndexType const& operator[](const Type& a_Value);
        Type const& operator[](const IndexType& a_Index) const;
        IndexType const& at(const Type& a_Value) const { return mapA.at(std::hash<Type> {}(a_Value)); }
        Type const& at(const IndexType& a_Index) const { return mapB.at(a_Index); }
        bool contains(const IndexType& a_Index) const { return mapB.contains(a_Index); }
        bool contains(const Type& a_Value) const { return mapA.contains(std::hash<Type> {}(a_Value)); }
        void erase(const IndexType& a_Index);
        void erase(const Type& a_Value);
        MapBType::const_iterator erase(const MapBType::const_iterator& a_It);
        std::pair<IndexType, bool> insert(const Type& a_Value);
        void clear();
        size_t size() const;
        void reserve(const size_t& a_Size);
        MapBType::const_iterator begin() const;
        MapBType::const_iterator end() const;

    private:
        IndexType id = 0;
        MapAType mapA;
        MapBType mapB;
    };
    class SymetricMatrix : public std::array<double, 10> {
    public:
        SymetricMatrix(double c = 0);
        SymetricMatrix(double a, double b, double c, double d);
        double Error(const glm::vec3& a_V) const;
        SymetricMatrix& operator+=(const SymetricMatrix& n);
        SymetricMatrix operator+(const SymetricMatrix& n) const;
    };
    class Vertex {
    public:
        bool operator!=(const Vertex& a_Rhs) const;
        bool operator==(const Vertex& a_Rhs) const;
        glm::vec3 position;
        mutable glm::vec3 normal;
        mutable glm::vec4 tangent;
        mutable glm::vec2 texCoord0;
        mutable glm::vec2 texCoord1;
        mutable glm::vec2 texCoord2;
        mutable glm::vec2 texCoord3;
        mutable glm::vec3 color;
        mutable glm::vec4 joints;
        mutable glm::vec4 weights;
        mutable SymetricMatrix quadricMatrix = {};
    };
    class Triangle {
    public:
        bool operator==(const Triangle& a_Lhs) const;
        std::array<uint64_t, 3> vertice;
        mutable SymetricMatrix quadricMatrix = {};
        mutable Component::Plane plane       = {};
        mutable glm::vec3 originalNormal     = {};
        mutable bool collapsed               = false;
    };
    friend std::hash<Triangle>;
    class Pair {
    public:
        Pair(const uint64_t& a_V0, const uint64_t& a_V1)
            : vertice({ std::min(a_V0, a_V1), std::max(a_V0, a_V1) })
        {
        }
        bool operator==(const Pair& a_Lhs) const;
        std::array<uint64_t, 2> vertice;
        mutable double contractionCost = 0;
        mutable Vertex target          = {};
    };

    friend std::hash<Pair>;
    class Reference {
    public:
        Reference();
        std::unordered_set<uint64_t> pairs;
        std::unordered_set<uint64_t> triangles;
    };

    friend std::hash<Vertex>;
    using posType = glm::vec3;
    using norType = glm::vec3;
    using tanType = glm::vec4;
    using texType = glm::vec2;
    using colType = glm::vec3;
    using joiType = glm::vec4;
    using weiType = glm::vec4;
    const bool _hasNormals;
    const bool _hasTangents;
    const bool _hasTexCoord0;
    const bool _hasTexCoord1;
    const bool _hasTexCoord2;
    const bool _hasTexCoord3;
    const bool _hasColors;
    const bool _hasJoints;
    const bool _hasWeights;
    std::unordered_map<uint64_t, Reference> _references;
    BiMap<Vertex> _vertice;
    BiMap<Triangle> _triangles;
    BiMap<Pair> _pairs;

    std::vector<uint64_t> _pairIndice;

    template <typename Accessor>
    void _FromIndexed(const std::shared_ptr<Primitive>& a_Primitive, const Accessor& a_Indice);
    void _PushTriangle(const std::shared_ptr<Primitive>& a_Primitive, const std::array<uint32_t, 3>& a_Indice);

    uint64_t _Triangle_Insert(const Triangle& a_Triangle);
    void _Triangle_Delete(const uint64_t& a_TriangleI);
    void _Triangle_Update(const uint64_t& a_TriangleI);
    void _Triangle_Update(Triangle& a_Triangle) const;
    bool _Triangle_IsCollapsed(const uint64_t& a_Triangle) const;
    bool _Triangle_IsCollapsed(const Triangle& a_Triangle) const;
    void _Triangle_HandleInversion(Triangle& a_Triangle) const;

    uint64_t _Vertex_Insert(const Vertex& a_V);
    void _Vertex_Delete(const uint64_t& a_I);
    Vertex _Vertex_Merge(const uint64_t& a_I0, const uint64_t& a_I1, const float& a_X = 0.5f);
    Vertex _Vertex_Merge(const Vertex& a_V0, const Vertex& a_V1, const float& a_X = 0.5f);

    uint64_t _Pair_Insert(const uint64_t& a_VertexI0, const uint64_t& a_VertexI1);
    uint64_t _Pair_Insert(const Pair& a_Pair);
    void _Pair_Delete(const uint64_t& a_PairI);
    void _Pair_Update(const uint64_t& a_PairI);
    void _Pair_Sort();

    bool _CheckReferencesValidity() const;

    std::shared_ptr<Primitive> _ReconstructPrimitive() const;
};
}
