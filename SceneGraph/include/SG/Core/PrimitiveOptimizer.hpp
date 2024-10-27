#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Component/BoundingVolume.hpp>

#include <glm/mat4x4.hpp>

#include <array>
#include <memory>
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
    PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive, const float& a_DistanceThreshold = 0.1f);
    std::shared_ptr<Primitive> operator()(const float& a_Aggressivity = 0.5f, const float& a_MaxCompressionCost = std::numeric_limits<float>::infinity());
    std::shared_ptr<Primitive> result;

private:
    class SymetricMatrix : public std::array<double, 10> {
    public:
        SymetricMatrix(double c = 0);
        SymetricMatrix(double a, double b, double c, double d);
        double Error(const glm::vec4& a_V) const;
        double det(int a11, int a12, int a13,
            int a21, int a22, int a23,
            int a31, int a32, int a33) const
        {
            return at(a11) * at(a22) * at(a33) + at(a13) * at(a21) * at(a32) + at(a12) * at(a23) * at(a31)
                - at(a13) * at(a22) * at(a31) - at(a11) * at(a23) * at(a32) - at(a12) * at(a21) * at(a33);
        }
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
    // Pair is a pair of positions indice
    class Pair {
    public:
        std::array<uint64_t, 2> positions;
        bool operator==(const Pair& a_Lhs) const;
        double contractionCost = 0;
        uint8_t target         = -1; // 0 == v0; 1 == v1; 2 = (v1 + v2) / 2
    };
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
    std::vector<posType> _positions;
    std::vector<norType> _normals;
    std::vector<tanType> _tangents;
    std::vector<texType> _texCoords;
    std::vector<colType> _colors;
    std::vector<joiType> _joints;
    std::vector<weiType> _weights;
    std::vector<Vertex> _vertice;
    std::vector<Triangle> _triangles;
    std::vector<Pair> _pairs;
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
    VertexData _ComputeTarget(const Pair& a_Pair, const uint64_t& a_VertexI0, const uint64_t& a_VertexI1);
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
