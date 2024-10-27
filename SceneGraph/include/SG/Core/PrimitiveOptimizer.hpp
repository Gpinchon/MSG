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
class SymetricMatrix : public std::array<double, 10> {
public:
    SymetricMatrix(double c = 0) { fill(c); }
    SymetricMatrix(double a, double b, double c, double d)
        : std::array<double, 10>(
              { a * a, a * b, a * c, a * d,
                  b * b, b * c, b * d,
                  c * c, c * d,
                  d * d })
    {
    }
    auto det(int a11, int a12, int a13,
        int a21, int a22, int a23,
        int a31, int a32, int a33)
    {
        return at(a11) * at(a22) * at(a33) + at(a13) * at(a21) * at(a32) + at(a12) * at(a23) * at(a31)
            - at(a13) * at(a22) * at(a31) - at(a11) * at(a23) * at(a32) - at(a12) * at(a21) * at(a33);
    }
    auto& operator+=(const SymetricMatrix& n)
    {
        for (uint8_t i = 0; i < size(); ++i)
            at(i) += n.at(i);
        return *this;
    }
    auto operator+(const SymetricMatrix& n) const { return SymetricMatrix { *this } += n; }
};

class Vertex {
public:
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

    bool operator!=(const Vertex& a_Rhs) const
    {
        return position != a_Rhs.position
            || normal != a_Rhs.normal
            || tangent != a_Rhs.tangent
            || texCoord0 != a_Rhs.texCoord0
            || texCoord1 != a_Rhs.texCoord1
            || texCoord2 != a_Rhs.texCoord2
            || texCoord3 != a_Rhs.texCoord3
            || color != a_Rhs.color
            || joints != a_Rhs.joints
            || weights != a_Rhs.weights;
    }

    bool operator==(const Vertex& a_Rhs) const
    {
        return !(*this != a_Rhs);
    }
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

// Pair is a pair of positions indice
class Pair {
public:
    Pair(const uint64_t& a_Pos0, const uint64_t& a_Pos1)
        : positions({ a_Pos0, a_Pos1 })
    {
    }
    bool operator==(const Pair& a_Lhs) const { return positions == a_Lhs.positions; }
    uint8_t target; // 0 == v0; 1 == v1; 2 = (v1 + v2) / 2
    double error = 0;
    std::array<uint64_t, 2> positions;
};

// Triangle is a set of 3 vertex indice
class Triangle {
public:
    bool operator==(const Triangle& a_Lhs) const { return vertice == a_Lhs.vertice; }
    SymetricMatrix quadricMatrix;
    Component::Plane plane;
    std::array<uint64_t, 3> vertice;
    glm::vec3 originalNormal;
    bool collapsed = false;
};

class Reference {
public:
    Reference()
    {
        pairs.reserve(128);
        vertice.reserve(128);
        triangles.reserve(128);
    }
    SymetricMatrix quadricMatrix;
    std::unordered_set<uint64_t> pairs;
    std::unordered_set<uint64_t> vertice;
    std::unordered_set<uint64_t> triangles;
};

class PrimitiveOptimizer {
public:
    PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive, const float& a_Threshold = 0.1f);
    std::shared_ptr<Primitive> operator()(const float& a_Aggressivity = 0.5f, const float& a_MaxError = std::numeric_limits<float>::infinity());

    const std::shared_ptr<Primitive>& primitive;
    const bool hasNormals   = !primitive->GetNormals().empty();
    const bool hasTangents  = !primitive->GetTangent().empty();
    const bool hasTexCoord0 = !primitive->GetTexCoord0().empty();
    const bool hasTexCoord1 = !primitive->GetTexCoord1().empty();
    const bool hasTexCoord2 = !primitive->GetTexCoord2().empty();
    const bool hasTexCoord3 = !primitive->GetTexCoord3().empty();
    const bool hasColors    = !primitive->GetColors().empty();
    const bool hasJoints    = !primitive->GetJoints().empty();
    const bool hasWeights   = !primitive->GetWeights().empty();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec4> tangents;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec4> joints;
    std::vector<glm::vec4> weights;

    std::vector<Reference> references; // list the vertice/triangles referencing this position
    std::vector<uint64_t> pairIndice;

    std::vector<Vertex> vertice;
    std::vector<Triangle> triangles;
    std::vector<Pair> pairs;

private:
    template <typename Accessor>
    void _FromIndexed(const Accessor& a_Indice);
    void _PushTriangle(const std::array<uint32_t, 3>& a_Indice);
    bool _TriangleIsCollapsed(const Triangle& a_Triangle) const;
    uint64_t _InsertPosition(const glm::vec3& a_Position);
    uint64_t _InsertPair(const Pair& a_Pair);
    void _DeleteTriangle(const uint64_t& a_TriangleI);
    void _UpdatePair(Pair& a_Pair);
    void _UpdateTriangle(Triangle& a_Triangle);
    bool _CheckInversion(const Triangle& a_Triangle, const uint64_t& a_OldVertex, const uint64_t& a_NewVertex);
    VertexData _ComputeTarget(const Pair& a_Pair, const uint64_t& a_VertexI0, const uint64_t& a_VertexI1);
    VertexData _GetVertexData(const Vertex& a_V);
    VertexData _MergeVertice(const Vertex& a_V0, const Vertex& a_V1, const float& a_X = 0.5f);
    uint64_t _InsertVertexData(const VertexData& a_Vd);
    void _SortPairs();
    void _CheckValidity();
    std::shared_ptr<Primitive> _ReconstructPrimitive() const;
};
}
