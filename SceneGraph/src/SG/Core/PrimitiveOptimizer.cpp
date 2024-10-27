#include <SG/Core/Primitive.hpp>
#include <SG/Core/PrimitiveOptimizer.hpp>
#include <Tools/Debug.hpp>

#include <algorithm>
#include <numeric>
#include <unordered_set>

namespace TabGraph::SG {
struct AlmostEqual {
    template <typename T>
    bool operator()(const T& a_X, const T& a_Y) const
    {
        return glm::distance(a_X, a_Y) < FLT_EPSILON;
    }
};

template <unsigned L, typename T, bool Normalized = false>
static inline glm::vec<L, T> ConvertData(const SG::BufferAccessor& a_Accessor, size_t a_Index)
{
    const auto componentNbr = a_Accessor.GetComponentNbr();
    glm::vec<L, T> ret {};
    for (auto i = 0u; i < L && i < componentNbr; ++i) {
        switch (a_Accessor.GetComponentType()) {
        case SG::DataType::Int8:
            ret[i] = T(a_Accessor.template GetComponent<glm::int8>(a_Index, i));
            break;
        case SG::DataType::Uint8:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint8>(a_Index, i));
            break;
        case SG::DataType::Int16:
            ret[i] = T(a_Accessor.template GetComponent<glm::int16>(a_Index, i));
            break;
        case SG::DataType::Uint16:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint16>(a_Index, i));
            break;
        case SG::DataType::Int32:
            ret[i] = T(a_Accessor.template GetComponent<glm::int32>(a_Index, i));
            break;
        case SG::DataType::Uint32:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint32>(a_Index, i));
            break;
        case SG::DataType::Float16:
            ret[i] = T(glm::detail::toFloat32(a_Accessor.template GetComponent<glm::detail::hdata>(a_Index, i)));
            break;
        case SG::DataType::Float32:
            ret[i] = T(a_Accessor.template GetComponent<glm::f32>(a_Index, i));
            break;
        default:
            throw std::runtime_error("Unknown data format");
        }
    }
    if constexpr (Normalized) {
        if constexpr (L == 4)
            return glm::vec<L, T>(glm::normalize(glm::vec3(ret)), ret.w);
        else
            return glm::normalize(ret);
    } else
        return ret;
}

static auto TriangleNormal(const glm::vec3& a_P0, const glm::vec3& a_P1, const glm::vec3& a_P2)
{
    return glm::normalize(glm::cross(a_P2 - a_P0, a_P1 - a_P0));
}

template <typename T, typename Pred = std::equal_to<T>>
size_t InsertUnique(std::vector<T>& a_Vec, const T& a_Data, const Pred& a_Pred = {})
{
    auto it = std::find_if(a_Vec.begin(), a_Vec.end(), [a_Data, a_Pred](const auto& a_Val) { return a_Pred(a_Data, a_Val); });
    if (it == a_Vec.end()) {
        a_Vec.push_back(a_Data);
        return a_Vec.size() - 1;
    }
    return std::distance(a_Vec.begin(), it);
}

inline PrimitiveOptimizer::SymetricMatrix::SymetricMatrix(double c) { fill(c); }
inline PrimitiveOptimizer::SymetricMatrix::SymetricMatrix(double a, double b, double c, double d)
    : std::array<double, 10>(
          { a * a, a * b, a * c, a * d,
              b * b, b * c, b * d,
              c * c, c * d,
              d * d })
{
}
inline double PrimitiveOptimizer::SymetricMatrix::Error(const glm::vec3& a_V) const
{
    auto& q = *this;
    return (q[0] * a_V.x * a_V.x) + (2 * q[1] * a_V.x * a_V.y) + (2 * q[2] * a_V.x * a_V.z) + (2 * q[3] * a_V.x)
        + (q[4] * a_V.y * a_V.y) + (2 * q[5] * a_V.y * a_V.z) + (2 * q[6] * a_V.y)
        + (q[7] * a_V.z * a_V.z) + (2 * q[8] * a_V.z)
        + (q[9]);
}
inline PrimitiveOptimizer::SymetricMatrix& PrimitiveOptimizer::SymetricMatrix::operator+=(const SymetricMatrix& n)
{
    for (uint8_t i = 0; i < size(); ++i)
        at(i) += n.at(i);
    return *this;
}
PrimitiveOptimizer::SymetricMatrix PrimitiveOptimizer::SymetricMatrix::operator+(const SymetricMatrix& n) const { return SymetricMatrix { *this } += n; }
bool PrimitiveOptimizer::Vertex::operator!=(const Vertex& a_Rhs) const
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
bool PrimitiveOptimizer::Vertex::operator==(const Vertex& a_Rhs) const { return !(*this != a_Rhs); }
bool PrimitiveOptimizer::Triangle::operator==(const Triangle& a_Lhs) const { return vertice == a_Lhs.vertice; }
bool PrimitiveOptimizer::Pair::operator==(const Pair& a_Lhs) const { return positions == a_Lhs.positions; }

inline PrimitiveOptimizer::Reference::Reference()
{
    pairs.reserve(128);
    vertice.reserve(128);
    triangles.reserve(128);
}

bool PrimitiveOptimizer::_CheckReferencesValidity()
{
    for (uint64_t vertexI = 0; vertexI < _vertice.size(); vertexI++) {
        const auto& vertex      = _vertice[vertexI];
        const auto& positionI   = vertex.position;
        const bool isReferenced = _references[positionI].vertice.contains(vertexI);
        if (!isReferenced)
            return false;
    }
    for (uint64_t triangleI = 0; triangleI < _triangles.size(); triangleI++) {
        const auto& triangle = _triangles[triangleI];
        for (const auto& vertexI : triangle.vertice) {
            const auto& vertex      = _vertice[vertexI];
            const auto& positionI   = vertex.position;
            const bool isReferenced = _references[positionI].triangles.contains(triangleI);
            if (triangle.collapsed && isReferenced) // this triangle shouldn't be referenced anymore
                return false;
            else if (!triangle.collapsed && !isReferenced)
                return false;
        }
    }
    for (const auto& pairI : _pairIndice) {
        const auto& pair = _pairs[pairI];
        for (const auto& positionI : pair.positions) {
            const bool isReferenced = _references[positionI].pairs.contains(pairI);
            if (!isReferenced)
                return false;
        }
    }
    for (uint64_t refI = 0; refI < _references.size(); refI++) {
        const auto& ref = _references[refI];
        for (const auto& vertexI : ref.vertice) {
            const auto& vertex    = _vertice[vertexI];
            const auto& positionI = vertex.position;
            if (positionI != refI) // if this is true this vertex is misplaced
                return false;
        }
        for (const auto& triangleI : ref.triangles) {
            const auto& triangle = _triangles[triangleI];
            for (const auto& vertexI : triangle.vertice) {
                const auto& vertex    = _vertice[vertexI];
                const auto& positionI = vertex.position;
                if (positionI != refI) // if this is true this vertex is misplaced
                    return false;
            }
        }
        for (const auto& pairI : ref.pairs) {
            const auto& pair = _pairs[pairI];
            for (auto& positionI : pair.positions) {
                if (positionI != refI) // if this is true this pair is misplaced
                    return false;
            }
        }
    }
    return true;
}

PrimitiveOptimizer::PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive, const float& a_DistanceThreshold)
    : _hasNormals(!a_Primitive->GetNormals().empty())
    , _hasTangents(!a_Primitive->GetTangent().empty())
    , _hasTexCoord0(!a_Primitive->GetTexCoord0().empty())
    , _hasTexCoord1(!a_Primitive->GetTexCoord1().empty())
    , _hasTexCoord2(!a_Primitive->GetTexCoord2().empty())
    , _hasTexCoord3(!a_Primitive->GetTexCoord3().empty())
    , _hasColors(!a_Primitive->GetColors().empty())
    , _hasJoints(!a_Primitive->GetJoints().empty())
    , _hasWeights(!a_Primitive->GetWeights().empty())
{
    if (a_Primitive->GetDrawingMode() != SG::Primitive::DrawingMode::Triangles) {
        errorLog("Mesh optimization only available for triangulated meshes");
        return;
    }
    _positions.reserve(a_Primitive->GetPositions().GetSize());
    _references.reserve(_positions.size());
    _normals.reserve(a_Primitive->GetNormals().GetSize());
    _tangents.reserve(a_Primitive->GetTangent().GetSize());
    _texCoords.reserve(a_Primitive->GetTexCoord0().GetSize());
    _colors.reserve(a_Primitive->GetColors().GetSize());
    _joints.reserve(a_Primitive->GetJoints().GetSize());
    _weights.reserve(a_Primitive->GetWeights().GetSize());

    consoleStream << "Loading mesh...";
    if (!a_Primitive->GetIndices().empty()) {
        _triangles.reserve(a_Primitive->GetIndices().GetSize() / 3);
        if (a_Primitive->GetIndices().GetComponentType() == SG::DataType::Uint32)
            _FromIndexed(a_Primitive, TypedBufferAccessor<uint32_t>(a_Primitive->GetIndices()));
        else if (a_Primitive->GetIndices().GetComponentType() == SG::DataType::Uint16)
            _FromIndexed(a_Primitive, TypedBufferAccessor<uint16_t>(a_Primitive->GetIndices()));
    } else {
        for (uint32_t i = 0; i < a_Primitive->GetPositions().GetSize(); i += 3)
            _PushTriangle(a_Primitive, { i + 0, i + 1, i + 2 });
    }
    consoleStream << "Loading done.\n";
    consoleStream << "Vertice count   : " << _vertice.size() << '\n';
    consoleStream << "Triangles count : " << _triangles.size() << '\n';
    consoleStream << "Adding mesh edges to valid pairs...\n";
    for (uint64_t triangleI = 0; triangleI < _triangles.size(); triangleI++) {
        const auto& triangle = _triangles[triangleI];
        const auto& v0       = _vertice[triangle.vertice[0]];
        const auto& v1       = _vertice[triangle.vertice[1]];
        const auto& v2       = _vertice[triangle.vertice[2]];
        auto pairI0          = _Pair_Insert(v0.position, v1.position);
        auto pairI1          = _Pair_Insert(v1.position, v2.position);
        auto pairI2          = _Pair_Insert(v2.position, v0.position);
        // Compute Q matrices
        for (const auto& vertexI : triangle.vertice) {
            const auto& positionI = _vertice[vertexI].position;
            _references[positionI].quadricMatrix += triangle.quadricMatrix;
        }
    }
    consoleStream << "Adding close vertice to valid pairs, distance threshold : " << a_DistanceThreshold << '\n';
    for (uint64_t position0I = 0; position0I < _positions.size(); ++position0I) {
        auto& position0 = _positions[position0I];
        for (uint64_t position1I = 0; position1I < _positions.size(); ++position1I) {
            auto& position1 = _positions[position1I];
            if (glm::distance(position0, position1) < a_DistanceThreshold)
                _Pair_Insert(position0I, position1I);
        }
    }
    // Initiate pair indice and sort
    _pairIndice.resize(_pairs.size());
    std::iota(_pairIndice.begin(), _pairIndice.end(), 0);
    _Pair_Sort();
    assert(_CheckReferencesValidity());
}

std::shared_ptr<Primitive> PrimitiveOptimizer::operator()(const float& a_Aggressivity, const float& a_MaxCompressionCost)
{
    const auto targetCompressionRatio = (1 - std::clamp(a_Aggressivity, 0.f, 1.f));
    const auto targetTrianglesCount   = std::max(uint32_t(_triangles.size() * targetCompressionRatio), 3u);
    auto currentTrianglesCount        = _triangles.size();
    consoleStream << "Starting mesh compression..." << '\n';
    consoleStream << "Agressivity             : " << a_Aggressivity << '\n';
    consoleStream << "Max compression cost    : " << a_MaxCompressionCost << '\n';
    consoleStream << "Input triangles count   : " << currentTrianglesCount << '\n';
    consoleStream << "Target triangles count  : " << targetTrianglesCount << '\n';
    std::unordered_set<uint64_t> verticeToTransfer;
    std::unordered_set<uint64_t> trianglesToTransfer;
    std::unordered_set<uint64_t> pairsToTransfer;
    verticeToTransfer.reserve(1024);
    trianglesToTransfer.reserve(1024);
    pairsToTransfer.reserve(1024);
    while (currentTrianglesCount > targetTrianglesCount) {
        const auto& pairToCollapseI = _pairIndice.back();
        const auto& pairToCollapse  = _pairs[pairToCollapseI];
        _pairIndice.pop_back();
        if (pairToCollapse.contractionCost > a_MaxCompressionCost) {
            consoleStream << "Cannot optimize further : Max error reached " << pairToCollapse.contractionCost << "/" << a_MaxCompressionCost << "\n";
            break;
        }
        auto newPositionI = _InsertPosition(pairToCollapse.targetPos);
        for (const auto& positionI : pairToCollapse.positions) {
            auto& ref = _references[positionI];
            verticeToTransfer.insert(ref.vertice.begin(), ref.vertice.end());
            trianglesToTransfer.insert(ref.triangles.begin(), ref.triangles.end());
            pairsToTransfer.insert(ref.pairs.begin(), ref.pairs.end());
            ref.vertice.clear();
            ref.triangles.clear();
            ref.pairs.clear();
            ref.quadricMatrix = {};
        }
        auto& ref = _references[newPositionI];
        for (const auto& vertexI : verticeToTransfer) {
            auto& vertex    = _vertice[vertexI];
            vertex.position = newPositionI;
            ref.vertice.insert(vertexI);
        }
        verticeToTransfer.clear();
        for (const auto& triangleI : trianglesToTransfer) {
            auto& triangle = _triangles[triangleI];
            _Triangle_Update(triangleI);
            if (triangle.collapsed) {
                _Triangle_Delete(triangleI);
                currentTrianglesCount--;
            } else {
                // Check for inversion & do inversion if needed
                if (glm::dot(triangle.plane.GetNormal(), triangle.originalNormal) < 0) {
                    std::swap(triangle.vertice[0], triangle.vertice[2]);
                    _Triangle_Update(triangleI);
                }
                ref.triangles.insert(triangleI);
            }
        }
        trianglesToTransfer.clear();
        for (const auto& pairI : pairsToTransfer) {
            auto& pair = _pairs[pairI];
            if (pairToCollapseI == pairI)
                continue;
            for (const auto& oldPositionI : pairToCollapse.positions)
                std::replace(pair.positions.begin(), pair.positions.end(), oldPositionI, newPositionI);
            ref.pairs.insert(pairI);
        }
        pairsToTransfer.clear();
        ref.quadricMatrix = {};
        for (const auto& triangleI : ref.triangles) {
            const auto& triangle = _triangles[triangleI];
            ref.quadricMatrix += triangle.quadricMatrix;
        }
        for (const auto& pairI : ref.pairs)
            _Pair_Update(pairI);
        _Pair_Sort();
        assert(_CheckReferencesValidity());
    }
    const auto inputCount = _triangles.size();
    std::erase_if(_triangles, [this](const auto& a_Triangle) { return a_Triangle.collapsed; });
    const auto outputCount = _triangles.size();
    consoleStream << "Output triangle count   : " << outputCount << '\n';
    consoleStream << "Compression ratio       : " << outputCount / float(inputCount) << '\n';
    return _ReconstructPrimitive();
}

template <typename Accessor>
inline void PrimitiveOptimizer::_FromIndexed(const std::shared_ptr<Primitive>& a_Primitive, const Accessor& a_Indice)
{
    for (uint32_t i = 0; i < a_Indice.GetSize(); i += 3)
        _PushTriangle(a_Primitive, { a_Indice.at(i + 0), a_Indice.at(i + 1), a_Indice.at(i + 2) });
}

void PrimitiveOptimizer::_PushTriangle(const std::shared_ptr<Primitive>& a_Primitive, const std::array<uint32_t, 3>& a_Indice)
{
    Triangle triangle = {};
    for (uint32_t i = 0; i < 3; i++) {
        VertexData vertexData = {};
        vertexData.position   = ConvertData<posType::length(), posType::value_type>(a_Primitive->GetPositions(), a_Indice[i]);
        vertexData.normal     = _hasNormals ? ConvertData<norType::length(), norType::value_type, true>(a_Primitive->GetNormals(), a_Indice[i]) : norType {};
        vertexData.tangent    = _hasTangents ? ConvertData<tanType::length(), tanType::value_type>(a_Primitive->GetTangent(), a_Indice[i]) : tanType {};
        vertexData.texCoord0  = _hasTexCoord0 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord0(), a_Indice[i]) : texType {};
        vertexData.texCoord1  = _hasTexCoord1 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord1(), a_Indice[i]) : texType {};
        vertexData.texCoord2  = _hasTexCoord2 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord2(), a_Indice[i]) : texType {};
        vertexData.texCoord3  = _hasTexCoord3 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord3(), a_Indice[i]) : texType {};
        vertexData.color      = _hasColors ? ConvertData<colType::length(), colType::value_type>(a_Primitive->GetColors(), a_Indice[i]) : colType {};
        vertexData.joints     = _hasJoints ? ConvertData<joiType::length(), joiType::value_type>(a_Primitive->GetJoints(), a_Indice[i]) : joiType {};
        vertexData.weights    = _hasWeights ? ConvertData<weiType::length(), weiType::value_type>(a_Primitive->GetWeights(), a_Indice[i]) : weiType {};
        auto vertexI          = _InsertVertexData(vertexData);
        const auto& vertex    = _vertice[vertexI];
        triangle.vertice[i]   = vertexI;
        _references[vertex.position].vertice.insert(InsertUnique(_vertice, vertex));
    }
    _Triangle_Update(triangle);
    // Don't keep already collapsed triangles
    if (triangle.collapsed)
        return;
    triangle.originalNormal = triangle.plane.GetNormal();
    auto triangleI          = InsertUnique(_triangles, triangle);
    for (const auto& vertexI : triangle.vertice) {
        const auto& vertex = _vertice[vertexI];
        _references[vertex.position].triangles.insert(triangleI);
    }
}

uint64_t PrimitiveOptimizer::_InsertPosition(const glm::vec3& a_Position)
{
    auto positionI = InsertUnique(_positions, a_Position, AlmostEqual {});
    _references.resize(_positions.size());
    return positionI;
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const uint64_t& a_TriangleI) const
{
    return _Triangle_IsCollapsed(_triangles[a_TriangleI]);
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const Triangle& a_Triangle) const
{
    const auto& posI0 = _vertice[a_Triangle.vertice[0]].position;
    const auto& posI1 = _vertice[a_Triangle.vertice[1]].position;
    const auto& posI2 = _vertice[a_Triangle.vertice[2]].position;
    return posI0 == posI1
        || posI0 == posI2
        || posI1 == posI2;
}

void PrimitiveOptimizer::_Triangle_Delete(const uint64_t& a_TriangleI)
{
    auto& triangle = _triangles[a_TriangleI];
    for (auto& vertexI : triangle.vertice) {
        const auto& positionI = _vertice[vertexI].position;
        _references[positionI].triangles.erase(a_TriangleI);
    }
}

void PrimitiveOptimizer::_Triangle_Update(const uint64_t& a_TriangleI)
{
    _Triangle_Update(_triangles[a_TriangleI]);
}

void PrimitiveOptimizer::_Triangle_Update(Triangle& a_Triangle) const
{
    const auto& v0           = _vertice[a_Triangle.vertice[0]];
    const auto& v1           = _vertice[a_Triangle.vertice[1]];
    const auto& v2           = _vertice[a_Triangle.vertice[2]];
    const auto& p0           = _positions[v0.position];
    const auto& p1           = _positions[v1.position];
    const auto& p2           = _positions[v2.position];
    a_Triangle.plane         = Component::Plane(p0, glm::normalize(glm::cross(p1 - p0, p2 - p0)));
    a_Triangle.quadricMatrix = SymetricMatrix(a_Triangle.plane[0], a_Triangle.plane[1], a_Triangle.plane[2], a_Triangle.plane[3]);
    a_Triangle.collapsed     = _Triangle_IsCollapsed(a_Triangle);
}

void PrimitiveOptimizer::_Triangle_HandleInversion(const uint64_t& a_TriangleI)
{
    auto& triangle = _triangles[a_TriangleI];
    if (glm::dot(triangle.plane.GetNormal(), triangle.originalNormal) < 0) {
        std::swap(triangle.vertice[0], triangle.vertice[2]);
        _Triangle_Update(a_TriangleI);
    }
}

PrimitiveOptimizer::VertexData PrimitiveOptimizer::_GetVertexData(const Vertex& a_V)
{
    return {
        .position  = _positions[a_V.position],
        .normal    = _hasNormals ? _normals[a_V.normal] : norType {},
        .tangent   = _hasTangents ? _tangents[a_V.tangent] : tanType {},
        .texCoord0 = _hasTexCoord0 ? _texCoords[a_V.texCoord0] : texType {},
        .texCoord1 = _hasTexCoord1 ? _texCoords[a_V.texCoord1] : texType {},
        .texCoord2 = _hasTexCoord2 ? _texCoords[a_V.texCoord2] : texType {},
        .texCoord3 = _hasTexCoord3 ? _texCoords[a_V.texCoord3] : texType {},
        .color     = _hasColors ? _colors[a_V.color] : colType {},
        .joints    = _hasJoints ? _joints[a_V.joints] : joiType {},
        .weights   = _hasWeights ? _weights[a_V.weights] : weiType {},
    };
}

PrimitiveOptimizer::VertexData PrimitiveOptimizer::_MergeVertice(const Vertex& a_V0, const Vertex& a_V1, const float& a_X)
{
    auto vd0 = _GetVertexData(a_V0);
    auto vd1 = _GetVertexData(a_V1);
    return {
        .position  = glm::mix(vd0.position, vd1.position, a_X),
        .normal    = glm::mix(vd0.normal, vd1.normal, a_X),
        .tangent   = glm::mix(vd0.tangent, vd1.tangent, a_X),
        .texCoord0 = glm::mix(vd0.texCoord0, vd1.texCoord0, a_X),
        .texCoord1 = glm::mix(vd0.texCoord1, vd1.texCoord1, a_X),
        .texCoord2 = glm::mix(vd0.texCoord2, vd1.texCoord2, a_X),
        .texCoord3 = glm::mix(vd0.texCoord3, vd1.texCoord3, a_X),
        .color     = glm::mix(vd0.color, vd1.color, a_X),
        .joints    = glm::mix(vd0.joints, vd1.joints, a_X),
        .weights   = glm::mix(vd0.weights, vd1.weights, a_X)
    };
}

uint64_t PrimitiveOptimizer::_InsertVertexData(const VertexData& a_Vd)
{
    Vertex v = {
        .position  = (int64_t)_InsertPosition(a_Vd.position),
        .normal    = _hasNormals ? (int64_t)InsertUnique(_normals, a_Vd.normal, AlmostEqual {}) : -1,
        .tangent   = _hasTangents ? (int64_t)InsertUnique(_tangents, a_Vd.tangent, AlmostEqual {}) : -1,
        .texCoord0 = _hasTexCoord0 ? (int64_t)InsertUnique(_texCoords, a_Vd.texCoord0, AlmostEqual {}) : -1,
        .texCoord1 = _hasTexCoord1 ? (int64_t)InsertUnique(_texCoords, a_Vd.texCoord1, AlmostEqual {}) : -1,
        .texCoord2 = _hasTexCoord2 ? (int64_t)InsertUnique(_texCoords, a_Vd.texCoord2, AlmostEqual {}) : -1,
        .texCoord3 = _hasTexCoord3 ? (int64_t)InsertUnique(_texCoords, a_Vd.texCoord3, AlmostEqual {}) : -1,
        .color     = _hasColors ? (int64_t)InsertUnique(_colors, a_Vd.color, AlmostEqual {}) : -1,
        .joints    = _hasJoints ? (int64_t)InsertUnique(_joints, a_Vd.joints, AlmostEqual {}) : -1,
        .weights   = _hasWeights ? (int64_t)InsertUnique(_weights, a_Vd.weights, AlmostEqual {}) : -1
    };
    auto vertexI = InsertUnique(_vertice, v);
    return *_references[v.position].vertice.insert(vertexI).first;
}

uint64_t PrimitiveOptimizer::_Pair_Insert(const uint64_t& a_Position0I, const uint64_t& a_Position1I)
{
    Pair pair;
    pair.positions = { a_Position0I, a_Position1I };
    return _Pair_Insert(pair);
}

uint64_t PrimitiveOptimizer::_Pair_Insert(const Pair& a_Pair)
{
    auto it = std::find_if(_pairs.begin(), _pairs.end(), [a_Pair](const auto& a_Val) { return a_Val == a_Pair; });
    if (it == _pairs.end()) {
        _pairs.push_back(a_Pair);
        auto pairI = _pairs.size() - 1;
        _references[a_Pair.positions[0]].pairs.insert(pairI);
        _references[a_Pair.positions[1]].pairs.insert(pairI);
        _Pair_Update(pairI); // Compute initial contraction cost
        return pairI;
    }
    return std::distance(_pairs.begin(), it);
}

void PrimitiveOptimizer::_Pair_Update(Pair& a_Pair)
{
    const auto& posI0 = a_Pair.positions[0];
    const auto& posI1 = a_Pair.positions[1];
    const auto& p0    = _positions[posI0];
    const auto& p1    = _positions[posI1];
    auto q            = _references[posI0].quadricMatrix + _references[posI1].quadricMatrix;
    double minError   = std::numeric_limits<double>::max();
    for (const auto& p : { p0, p1, (p0 + p1) * 0.5f }) {
        double error = q.Error(p);
        if (error < minError) {
            a_Pair.contractionCost = minError = error;
            a_Pair.targetPos                  = p;
        }
    }
    // This comes from here and it works... I don't fully understand it right now though
    // https://github.com/jannessm/quadric-mesh-simplification/blob/05f783c2f5f9766834b02cb2190189b9438b8eff/quad_mesh_simplify/c/targets.c#L26
    glm::vec3 p01 = (p1 - p0) * 0.1f;
    for (int i = 0; i < 11; i++) {
        glm::vec3 p001 = p0 + p01 * float(i);
        double error   = q.Error(p001);
        if (error < minError) {
            a_Pair.contractionCost = minError = error;
            a_Pair.targetPos                  = p001;
        }
    }
}

void PrimitiveOptimizer::_Pair_Update(const uint64_t& a_PairI)
{
    return _Pair_Update(_pairs[a_PairI]);
}

void PrimitiveOptimizer::_Pair_Sort()
{
    std::sort(_pairIndice.begin(), _pairIndice.end(), [&pairs = _pairs](auto& a_A, auto& a_B) {
        return pairs[a_A].contractionCost > pairs[a_B].contractionCost;
    });
}

std::shared_ptr<Primitive> PrimitiveOptimizer::_ReconstructPrimitive() const
{
    // Generate new primitive
    auto vertexCount    = _triangles.size() * 3;
    auto positionsSize  = vertexCount * sizeof(posType);
    auto normalsSize    = _hasNormals ? vertexCount * sizeof(norType) : 0u;
    auto tangentsSize   = _hasTangents ? vertexCount * sizeof(tanType) : 0u;
    auto texCoords0Size = _hasTexCoord0 ? vertexCount * sizeof(texType) : 0u;
    auto texCoords1Size = _hasTexCoord1 ? vertexCount * sizeof(texType) : 0u;
    auto texCoords2Size = _hasTexCoord2 ? vertexCount * sizeof(texType) : 0u;
    auto texCoords3Size = _hasTexCoord3 ? vertexCount * sizeof(texType) : 0u;
    auto colorsSize     = _hasColors ? vertexCount * sizeof(colType) : 0u;
    auto jointsSize     = _hasJoints ? vertexCount * sizeof(joiType) : 0u;
    auto weightsSize    = _hasWeights ? vertexCount * sizeof(weiType) : 0u;

    int32_t positionsOffset  = 0;
    int32_t normalsOffset    = positionsOffset + positionsSize;
    int32_t tangentsOffset   = normalsOffset + normalsSize;
    int32_t texCoords0Offset = tangentsOffset + tangentsSize;
    int32_t texCoords1Offset = texCoords0Offset + texCoords0Size;
    int32_t texCoords2Offset = texCoords1Offset + texCoords1Size;
    int32_t texCoords3Offset = texCoords2Offset + texCoords2Size;
    int32_t colorsOffset     = texCoords3Offset + texCoords3Size;
    int32_t jointsOffset     = colorsOffset + colorsSize;
    int32_t weightsOffset    = jointsOffset + jointsSize;

    auto totalBufferSize = weightsOffset + weightsSize;

    std::vector<posType> positionsFinal;
    std::vector<norType> normalsFinal;
    std::vector<tanType> tangentsFinal;
    std::vector<texType> texCoords0Final;
    std::vector<texType> texCoords1Final;
    std::vector<texType> texCoords2Final;
    std::vector<texType> texCoords3Final;
    std::vector<colType> colorsFinal;
    std::vector<joiType> jointsFinal;
    std::vector<weiType> weightsFinal;
    positionsFinal.reserve(vertexCount);
    normalsFinal.reserve(vertexCount);
    tangentsFinal.reserve(vertexCount);
    texCoords0Final.reserve(vertexCount);
    texCoords1Final.reserve(vertexCount);
    texCoords2Final.reserve(vertexCount);
    texCoords3Final.reserve(vertexCount);
    colorsFinal.reserve(vertexCount);
    jointsFinal.reserve(vertexCount);
    weightsFinal.reserve(vertexCount);

    for (const auto& triangle : _triangles) {
        for (const auto& vertexI : triangle.vertice) {
            const auto& vertex = _vertice[vertexI];
            positionsFinal.push_back(_positions[vertex.position]);
            if (_hasNormals)
                normalsFinal.push_back(_normals[vertex.normal]);
            if (_hasTangents)
                tangentsFinal.push_back(_tangents[vertex.tangent]);
            if (_hasTexCoord0)
                texCoords0Final.push_back(_texCoords[vertex.texCoord0]);
            if (_hasTexCoord1)
                texCoords1Final.push_back(_texCoords[vertex.texCoord1]);
            if (_hasTexCoord2)
                texCoords2Final.push_back(_texCoords[vertex.texCoord2]);
            if (_hasTexCoord3)
                texCoords3Final.push_back(_texCoords[vertex.texCoord3]);
            if (_hasColors)
                colorsFinal.push_back(_colors[vertex.color]);
            if (_hasJoints)
                jointsFinal.push_back(_joints[vertex.joints]);
            if (_hasWeights)
                weightsFinal.push_back(_weights[vertex.weights]);
        }
    }

    auto buffer = std::make_shared<SG::Buffer>(totalBufferSize);

    std::memcpy(buffer->data() + positionsOffset, positionsFinal.data(), positionsSize);
    std::memcpy(buffer->data() + normalsOffset, normalsFinal.data(), normalsSize);
    std::memcpy(buffer->data() + tangentsOffset, tangentsFinal.data(), tangentsSize);
    std::memcpy(buffer->data() + texCoords0Offset, texCoords0Final.data(), texCoords0Size);
    std::memcpy(buffer->data() + texCoords1Offset, texCoords1Final.data(), texCoords1Size);
    std::memcpy(buffer->data() + texCoords2Offset, texCoords2Final.data(), texCoords2Size);
    std::memcpy(buffer->data() + texCoords3Offset, texCoords3Final.data(), texCoords3Size);
    std::memcpy(buffer->data() + colorsOffset, colorsFinal.data(), colorsSize);
    std::memcpy(buffer->data() + jointsOffset, jointsFinal.data(), jointsSize);
    std::memcpy(buffer->data() + weightsOffset, weightsFinal.data(), weightsSize);

    const auto bufferView = std::make_shared<BufferView>(buffer, 0, buffer->size());
    auto newPrimitive     = std::make_shared<SG::Primitive>();
    newPrimitive->SetPositions({ bufferView, positionsOffset, vertexCount, DataType::Float32, posType::length() });
    if (_hasNormals)
        newPrimitive->SetNormals({ bufferView, normalsOffset, vertexCount, DataType::Float32, norType::length() });
    if (_hasTangents)
        newPrimitive->SetTangent({ bufferView, tangentsOffset, vertexCount, DataType::Float32, tanType::length() });
    if (_hasTexCoord0)
        newPrimitive->SetTexCoord0({ bufferView, texCoords0Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasTexCoord1)
        newPrimitive->SetTexCoord1({ bufferView, texCoords1Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasTexCoord2)
        newPrimitive->SetTexCoord2({ bufferView, texCoords2Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasTexCoord3)
        newPrimitive->SetTexCoord3({ bufferView, texCoords3Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasColors)
        newPrimitive->SetColors({ bufferView, colorsOffset, vertexCount, DataType::Float32, colType::length() });
    if (_hasJoints)
        newPrimitive->SetJoints({ bufferView, jointsOffset, vertexCount, DataType::Float32, joiType::length() });
    if (_hasWeights)
        newPrimitive->SetWeights({ bufferView, weightsOffset, vertexCount, DataType::Float32, weiType::length() });
    newPrimitive->ComputeBoundingVolume();
    return newPrimitive;
}
}
