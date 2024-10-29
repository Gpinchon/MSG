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
    std::shared_ptr<Primitive> operator()(const float& a_Aggressivity = 0.5f, const float& a_MaxCompressionCost = std::numeric_limits<float>::infinity());
    std::shared_ptr<Primitive> result;

private:
    template <typename Type>
    class BiMap {
    public:
        using IndexType = uint64_t;
        using HashType  = uint64_t;
        using MapAType  = std::unordered_map<HashType, IndexType>;
        using MapBType  = std::unordered_map<IndexType, Type>;
        IndexType operator[](const Type& a_Value) const;
        Type& operator[](const IndexType& a_Index);
        const Type& operator[](const IndexType& a_Index) const;
        auto at(const Type& a_Value) const { return operator[](a_Value); }
        auto& at(const IndexType& a_Index) { return operator[](a_Index); }
        auto& at(const IndexType& a_Index) const { return operator[](a_Index); }
        void erase(const IndexType& a_Index);
        void erase(const Type& a_Value);
        std::pair<IndexType, bool> insert(const Type& a_Value);
        void clear();
        size_t size() const;
        void reserve(const size_t& a_Size);
        MapBType::const_iterator begin() const;
        MapBType::const_iterator end() const;
        MapBType::iterator begin();
        MapBType::iterator end();

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
        int64_t position  = -1;
        int64_t normal    = -1;
        int64_t tangent   = -1;
        int64_t texCoord0 = -1;
        int64_t texCoord1 = -1;
        int64_t texCoord2 = -1;
        int64_t texCoord3 = -1;
        int64_t color     = -1;
        int64_t joints    = -1;
        int64_t weights   = -1;
    };
    friend std::hash<Vertex>;
    // Triangle is a set of 3 vertex indice
    class Triangle {
    public:
        bool operator==(const Triangle& a_Lhs) const;
        SymetricMatrix quadricMatrix;
        Component::Plane plane;
        std::array<uint64_t, 3> vertice;
        glm::vec3 originalNormal;
        bool collapsed = false;
    };
    friend std::hash<Triangle>;
    // Pair is a pair of positions indice
    class Pair {
    public:
        std::array<uint64_t, 2> positions;
        bool operator==(const Pair& a_Lhs) const;
        double contractionCost = 0;
        glm::vec3 targetPos;
    };
    friend std::hash<Pair>;
    class Reference {
    public:
        Reference();
        SymetricMatrix quadricMatrix;
        std::unordered_set<uint64_t> pairs;
        std::unordered_set<uint64_t> vertice;
        std::unordered_set<uint64_t> triangles;
    };
    class VertexData {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec4 tangent;
        glm::vec2 texCoord0;
        glm::vec2 texCoord1;
        glm::vec2 texCoord2;
        glm::vec2 texCoord3;
        glm::vec3 color;
        glm::vec4 joints;
        glm::vec4 weights;
    };
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
    BiMap<posType> _positions;
    BiMap<norType> _normals;
    BiMap<tanType> _tangents;
    BiMap<texType> _texCoords;
    BiMap<colType> _colors;
    BiMap<joiType> _joints;
    BiMap<weiType> _weights;
    BiMap<Vertex> _vertice;
    BiMap<Triangle> _triangles;
    BiMap<Pair> _pairs;

    std::vector<uint64_t> _pairIndice;
    std::vector<Reference> _references; // list the vertice/triangles referencing this position

    template <typename Accessor>
    void _FromIndexed(const std::shared_ptr<Primitive>& a_Primitive, const Accessor& a_Indice);
    void _PushTriangle(const std::shared_ptr<Primitive>& a_Primitive, const std::array<uint32_t, 3>& a_Indice);
    bool _Triangle_IsCollapsed(const uint64_t& a_Triangle) const;
    bool _Triangle_IsCollapsed(const Triangle& a_Triangle) const;
    void _Triangle_HandleInversion(const uint64_t& a_TriangleI);
    void _Triangle_Delete(const uint64_t& a_TriangleI);
    void _Triangle_Update(const uint64_t& a_TriangleI);
    void _Triangle_Update(Triangle& a_Triangle) const;

    bool _CheckReferencesValidity();
    VertexData _GetVertexData(const Vertex& a_V);
    VertexData _MergeVertice(const Vertex& a_V0, const Vertex& a_V1, const float& a_X = 0.5f);
    uint64_t _InsertVertexData(const VertexData& a_Vd);
    uint64_t _InsertPosition(const glm::vec3& a_Position);

    uint64_t _Pair_Insert(const uint64_t& a_Position0I, const uint64_t& a_Position1I);
    uint64_t _Pair_Insert(const Pair& a_Pair);
    void _Pair_Update(Pair& a_Pair);
    void _Pair_Update(const uint64_t& a_PairI);
    void _Pair_Sort();

    std::shared_ptr<Primitive> _ReconstructPrimitive() const;
};
}
