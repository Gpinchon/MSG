#include <SG/Core/Primitive.hpp>
#include <SG/Core/PrimitiveOptimizer.hpp>
#include <Tools/Debug.hpp>
#include <Tools/HashCombine.hpp>

#include <glm/gtx/hash.hpp>

#include <algorithm>
#include <numeric>

namespace std {
template <>
struct hash<TabGraph::SG::PrimitiveOptimizer::Pair> {
    size_t operator()(const TabGraph::SG::PrimitiveOptimizer::Pair& a_Pair) const
    {
        std::size_t seed = 0;
        TABGRAPH_HASH_COMBINE(seed, a_Pair.vertice[0]);
        TABGRAPH_HASH_COMBINE(seed, a_Pair.vertice[1]);
        return seed;
    }
};
template <>
struct hash<TabGraph::SG::PrimitiveOptimizer::Triangle> {
    size_t operator()(const TabGraph::SG::PrimitiveOptimizer::Triangle& a_Triangle) const
    {
        std::size_t seed = 0;
        TABGRAPH_HASH_COMBINE(seed, a_Triangle.vertice[0]);
        TABGRAPH_HASH_COMBINE(seed, a_Triangle.vertice[1]);
        TABGRAPH_HASH_COMBINE(seed, a_Triangle.vertice[2]);
        return seed;
    }
};
template <>
struct hash<TabGraph::SG::PrimitiveOptimizer::Vertex> {
    size_t operator()(const TabGraph::SG::PrimitiveOptimizer::Vertex& a_Vertex) const
    {
        std::size_t seed = 0;
        TABGRAPH_HASH_COMBINE(seed, a_Vertex.position);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.normal);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.tangent);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord0);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord1);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord2);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.texCoord3);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.color);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.joints);
        // TABGRAPH_HASH_COMBINE(seed, a_Vertex.weights);
        return seed;
    }
};
}

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

template <typename Type>
auto PrimitiveOptimizer::BiMap<Type>::operator[](const Type& a_Value) -> IndexType const&
{
    auto itr = mapA.find(std::hash<Type> {}(a_Value));
    if (itr == mapA.end())
        return insert(a_Value).first;
    return itr->second;
}

template <typename Type>
auto PrimitiveOptimizer::BiMap<Type>::operator[](const IndexType& a_Index) const -> Type const&
{
    return at(a_Index);
}
template <typename Type>
void PrimitiveOptimizer::BiMap<Type>::erase(const IndexType& a_Index)
{
    const auto hash = std::hash<Type> {}(at(a_Index));
    if (mapB.contains(a_Index))
        assert(mapA.contains(hash));
    mapA.erase(hash);
    mapB.erase(a_Index);
    assert(mapA.size() == mapB.size());
}
template <typename Type>
void PrimitiveOptimizer::BiMap<Type>::erase(const Type& a_Value) { erase(at(a_Value)); }
template <typename Type>
auto PrimitiveOptimizer::BiMap<Type>::erase(const MapBType::const_iterator& a_It) -> MapBType::const_iterator
{
    mapA.erase(std::hash<Type> {}(a_It->second));
    return mapB.erase(a_It);
}
template <typename Type>
inline auto PrimitiveOptimizer::BiMap<Type>::insert(const Type& a_Value) -> std::pair<IndexType, bool>
{
    HashType aHash = std::hash<Type> {}(a_Value);
    auto aItr      = mapA.insert({ aHash, -1 });
    if (aItr.second) {
        // we just inserted this value
        aItr.first->second = id;
        mapB.insert({ id, a_Value });
        assert(mapA.size() == mapB.size());
        id++;
        return { aItr.first->second, true };
    }
    return { aItr.first->second, false };
}
template <typename Type>
void PrimitiveOptimizer::BiMap<Type>::clear()
{
    mapA.clear();
    mapB.clear();
}
template <typename Type>
size_t PrimitiveOptimizer::BiMap<Type>::size() const
{
    assert(mapA.size() == mapB.size());
    return mapA.size();
}
template <typename Type>
void PrimitiveOptimizer::BiMap<Type>::reserve(const size_t& a_Size)
{
    mapA.reserve(a_Size);
    mapB.reserve(a_Size);
}
template <typename Type>
auto PrimitiveOptimizer::BiMap<Type>::begin() const -> MapBType::const_iterator { return mapB.begin(); }
template <typename Type>
auto PrimitiveOptimizer::BiMap<Type>::end() const -> MapBType::const_iterator { return mapB.end(); }

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
bool PrimitiveOptimizer::Pair::operator==(const Pair& a_Lhs) const
{
    return (vertice[0] == a_Lhs.vertice[0] && vertice[1] == a_Lhs.vertice[1])
        || (vertice[0] == a_Lhs.vertice[1] && vertice[1] == a_Lhs.vertice[0]);
}

inline PrimitiveOptimizer::Reference::Reference()
{
    pairs.reserve(128);
    triangles.reserve(128);
}

bool PrimitiveOptimizer::_CheckReferencesValidity() const
{
    for (const auto& pair : _vertice) {
        const auto& vertexI     = pair.first;
        const bool isReferenced = _references.contains(vertexI);
        if (!isReferenced) {
            errorStream << "Vertex " << vertexI << " not referenced.\n";
            return false;
        }
    }
    for (const auto& pair : _triangles) {
        const auto& triangleI = pair.first;
        const auto& triangle  = pair.second;
        for (const auto& vertexI : triangle.vertice) {
            const bool isReferenced = _references.at(vertexI).triangles.contains(triangleI);
            if (triangle.collapsed && isReferenced) {
                errorStream << "Triangle " << triangleI << " collapsed but still referenced at " << vertexI << ".\n";
                return false;
            } else if (!triangle.collapsed && !isReferenced) {
                errorStream << "Triangle " << triangleI << " not collapsed but not referenced at " << vertexI << ".\n";
                return false;
            }
        }
    }
    for (const auto& pairI : _pairIndice) {
        const auto& pair = _pairs.at(pairI);
        for (const auto& vertexI : pair.vertice) {
            const bool isReferenced = _references.at(vertexI).pairs.contains(pairI);
            if (!isReferenced) {
                errorStream << "Pair " << pairI << " not referenced at " << vertexI << ".\n";
                return false;
            }
        }
    }
    for (auto& pair : _references) {
        const auto& refI = pair.first;
        const auto& ref  = pair.second;
        if (!_vertice.contains(refI)) {
            errorStream << "Vertex " << refI << " referenced but not in vertice list.\n";
            return false;
        }
        for (const auto& triangleI : ref.triangles) {
            const auto& triangle = _triangles.at(triangleI);
            bool valid           = false;
            for (const auto& vertexI : triangle.vertice) {
                if (vertexI == refI) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                errorStream << "Triangle " << triangleI << " referenced at " << refI << " but does not point to this reference.\n";
                return false;
            }
        }
        for (const auto& pairI : ref.pairs) {
            const auto& pair = _pairs.at(pairI);
            bool valid       = false;
            for (auto& vertexI : pair.vertice) {
                if (vertexI == refI) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                errorStream << "Pair " << pairI << " referenced at " << refI << " but does not point to this reference.\n";
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

    _vertice.reserve(a_Primitive->GetPositions().GetSize());
    _references.reserve(_vertice.size());

    consoleStream << "Loading mesh...\n";
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
        _Pair_Insert(triangle.vertice[0], triangle.vertice[1]);
        _Pair_Insert(triangle.vertice[1], triangle.vertice[2]);
        _Pair_Insert(triangle.vertice[2], triangle.vertice[0]);
        // Compute Q matrices
        for (const auto& vertexI : triangle.vertice) {
            _vertice.at(vertexI).quadricMatrix += triangle.quadricMatrix;
        }
    }
    consoleStream << "Adding close vertice to valid pairs, distance threshold : " << a_DistanceThreshold << '\n';
    for (const auto& vertex0 : _vertice) {
        for (const auto& vertex1 : _vertice) {
            if (vertex0.first == vertex1.first)
                continue;
            if (glm::distance(vertex0.second.position, vertex1.second.position) < a_DistanceThreshold)
                _Pair_Insert(vertex0.first, vertex1.first);
        }
    }
    //  Initiate pair indice and sort
    _pairIndice.resize(_pairs.size());
    std::iota(_pairIndice.begin(), _pairIndice.end(), 0);
    _Pair_Sort();
    assert(_CheckReferencesValidity());
}

std::shared_ptr<Primitive> PrimitiveOptimizer::operator()(const float& a_CompressionRatio, const float& a_MaxCompressionCost)
{
    const auto targetCompressionRatio = (1 - std::clamp(a_CompressionRatio, 0.f, 1.f));
    const auto targetTrianglesCount   = std::max(uint32_t(_triangles.size() * targetCompressionRatio), 3u);
    const auto inputTriangleCount     = _triangles.size();
    auto currentTrianglesCount        = _triangles.size();
    consoleStream << "Starting mesh compression..." << '\n';
    consoleStream << "Wanted compression ratio: " << targetCompressionRatio * 100.f << "%\n";
    consoleStream << "Max compression cost    : " << a_MaxCompressionCost << '\n';
    consoleStream << "Input triangles count   : " << currentTrianglesCount << '\n';
    consoleStream << "Target triangles count  : " << targetTrianglesCount << '\n';
    while (currentTrianglesCount > targetTrianglesCount) {
        const auto& pairToCollapseI = _pairIndice.back();
        const auto& pairToCollapse  = _pairs[pairToCollapseI];

        if (pairToCollapse.contractionCost > a_MaxCompressionCost) {
            consoleStream << "Cannot optimize further : max contraction cost reached " << pairToCollapse.contractionCost << "/" << a_MaxCompressionCost << "\n";
            break;
        }
        Reference refToMerge;
        for (uint8_t i = 0; i < 2; i++) {
            const auto& vertexI = pairToCollapse.vertice[i];
            auto& ref           = _references.at(vertexI);
            refToMerge.pairs.merge(std::move(ref.pairs));
            refToMerge.triangles.merge(std::move(ref.triangles));
        }
        auto newVertexI = _Vertex_Insert(pairToCollapse.target);
        auto& newRef    = _references.at(newVertexI);
        auto& newVertex = _vertice.at(newVertexI);
        refToMerge.pairs.merge(std::move(newRef.pairs));
        refToMerge.triangles.merge(std::move(newRef.triangles));
        for (auto& triangleI : refToMerge.triangles) {
            Triangle triangle = _triangles.at(triangleI);
            _Triangle_Delete(triangleI);
            for (auto& vertexI : triangle.vertice) {
                for (auto& oldVertexI : pairToCollapse.vertice)
                    std::replace(triangle.vertice.begin(), triangle.vertice.end(), oldVertexI, newVertexI);
            }
            _Triangle_Update(triangle);
            if (triangle.collapsed) {
                currentTrianglesCount--;
            } else {
                _Triangle_HandleInversion(triangle);
                _Triangle_Insert(triangle);
            }
        }

        for (const auto& pairI : refToMerge.pairs) {
            Pair pair = _pairs[pairI];
            _Pair_Delete(pairI);
            if (pairI == pairToCollapseI)
                continue;
            assert(pair.vertice[0] != pair.vertice[1]);
            for (const auto& oldVertexI : pairToCollapse.vertice)
                std::replace(pair.vertice.begin(), pair.vertice.end(), oldVertexI, newVertexI);
            if (pair.vertice[0] != pair.vertice[1])
                _Pair_Insert(pair);
        }
        for (const auto& triangleI : newRef.triangles) {
            newVertex.quadricMatrix += _triangles[triangleI].quadricMatrix;
        }
        for (const auto& pairI : newRef.pairs)
            _Pair_Update(pairI);
        _pairIndice.pop_back();
        _Pair_Sort();
        assert(_CheckReferencesValidity());
    }
    for (auto it = _vertice.begin(); it != _vertice.end();) {
        auto refIt = _references.find(it->first);
        if (refIt->second.triangles.empty()) {
            it = _vertice.erase(it);
            _references.erase(refIt);
        } else
            it++;
    }
    consoleStream << "Output triangle count   : " << _triangles.size() << '\n';
    consoleStream << "Compression ratio       : " << (1 - (_triangles.size() / float(inputTriangleCount))) * 100.f << "%\n";
    auto newPrimitive = _ReconstructPrimitive();
    return newPrimitive;
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
        Vertex vertex       = {};
        vertex.position     = ConvertData<posType::length(), posType::value_type>(a_Primitive->GetPositions(), a_Indice[i]);
        vertex.normal       = _hasNormals ? ConvertData<norType::length(), norType::value_type, true>(a_Primitive->GetNormals(), a_Indice[i]) : norType {};
        vertex.tangent      = _hasTangents ? ConvertData<tanType::length(), tanType::value_type>(a_Primitive->GetTangent(), a_Indice[i]) : tanType {};
        vertex.texCoord0    = _hasTexCoord0 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord0(), a_Indice[i]) : texType {};
        vertex.texCoord1    = _hasTexCoord1 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord1(), a_Indice[i]) : texType {};
        vertex.texCoord2    = _hasTexCoord2 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord2(), a_Indice[i]) : texType {};
        vertex.texCoord3    = _hasTexCoord3 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord3(), a_Indice[i]) : texType {};
        vertex.color        = _hasColors ? ConvertData<colType::length(), colType::value_type>(a_Primitive->GetColors(), a_Indice[i]) : colType {};
        vertex.joints       = _hasJoints ? ConvertData<joiType::length(), joiType::value_type>(a_Primitive->GetJoints(), a_Indice[i]) : joiType {};
        vertex.weights      = _hasWeights ? ConvertData<weiType::length(), weiType::value_type>(a_Primitive->GetWeights(), a_Indice[i]) : weiType {};
        triangle.vertice[i] = _Vertex_Insert(vertex);
    }
    _Triangle_Update(triangle);
    // Don't keep already collapsed triangles
    if (triangle.collapsed)
        return;
    triangle.originalNormal = triangle.plane.GetNormal();
    auto triangleI          = _triangles.insert(triangle).first;
    for (const auto& vertexI : triangle.vertice) {
        _references.at(vertexI).triangles.insert(triangleI);
    }
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const uint64_t& a_TriangleI) const
{
    return _Triangle_IsCollapsed(_triangles.at(a_TriangleI));
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const Triangle& a_Triangle) const
{
    const auto& posI0 = _vertice.at(a_Triangle.vertice[0]).position;
    const auto& posI1 = _vertice.at(a_Triangle.vertice[1]).position;
    const auto& posI2 = _vertice.at(a_Triangle.vertice[2]).position;
    return posI0 == posI1
        || posI0 == posI2
        || posI1 == posI2;
}

void PrimitiveOptimizer::_Triangle_Delete(const uint64_t& a_TriangleI)
{
    for (auto& vertexI : _triangles.at(a_TriangleI).vertice) {
        auto itr = _references.find(vertexI);
        if (itr != _references.end())
            itr->second.triangles.erase(a_TriangleI);
    }
    _triangles.erase(a_TriangleI);
}

void PrimitiveOptimizer::_Triangle_Update(const uint64_t& a_TriangleI)
{
    Triangle triangle = _triangles[a_TriangleI];
    _Triangle_Update(triangle);
}

void PrimitiveOptimizer::_Triangle_Update(Triangle& a_Triangle) const
{
    const auto& v0           = _vertice.at(a_Triangle.vertice[0]);
    const auto& v1           = _vertice.at(a_Triangle.vertice[1]);
    const auto& v2           = _vertice.at(a_Triangle.vertice[2]);
    const auto& p0           = v0.position;
    const auto& p1           = v1.position;
    const auto& p2           = v2.position;
    a_Triangle.plane         = Component::Plane(p0, glm::normalize(glm::cross(p1 - p0, p2 - p0)));
    a_Triangle.quadricMatrix = SymetricMatrix(a_Triangle.plane[0], a_Triangle.plane[1], a_Triangle.plane[2], a_Triangle.plane[3]);
    a_Triangle.collapsed     = _Triangle_IsCollapsed(a_Triangle);
}

uint64_t PrimitiveOptimizer::_Triangle_Insert(const Triangle& a_Triangle)
{
    auto triangleI = _triangles.insert(a_Triangle);
    for (const auto& vertexI : a_Triangle.vertice) {
        _references.at(vertexI).triangles.insert(triangleI.first);
    }
    return triangleI.first;
}

void PrimitiveOptimizer::_Triangle_HandleInversion(Triangle& a_Triangle) const
{
    if (glm::dot(a_Triangle.plane.GetNormal(), a_Triangle.originalNormal) < 0) {
        std::swap(a_Triangle.vertice[0], a_Triangle.vertice[2]);
        _Triangle_Update(a_Triangle);
    }
}

PrimitiveOptimizer::Vertex PrimitiveOptimizer::_Vertex_Merge(const uint64_t& a_I0, const uint64_t& a_I1, const float& a_X)
{
    return _Vertex_Merge(_vertice[a_I0], _vertice[a_I1], a_X);
}

PrimitiveOptimizer::Vertex PrimitiveOptimizer::_Vertex_Merge(const Vertex& a_V0, const Vertex& a_V1, const float& a_X)
{
    return {
        .position  = glm::mix(a_V0.position, a_V1.position, a_X),
        .normal    = glm::mix(a_V0.normal, a_V1.normal, a_X),
        .tangent   = glm::mix(a_V0.tangent, a_V1.tangent, a_X),
        .texCoord0 = glm::mix(a_V0.texCoord0, a_V1.texCoord0, a_X),
        .texCoord1 = glm::mix(a_V0.texCoord1, a_V1.texCoord1, a_X),
        .texCoord2 = glm::mix(a_V0.texCoord2, a_V1.texCoord2, a_X),
        .texCoord3 = glm::mix(a_V0.texCoord3, a_V1.texCoord3, a_X),
        .color     = glm::mix(a_V0.color, a_V1.color, a_X),
        .joints    = glm::mix(a_V0.joints, a_V1.joints, a_X),
        .weights   = glm::mix(a_V0.weights, a_V1.weights, a_X)
    };
}

uint64_t PrimitiveOptimizer::_Vertex_Insert(const Vertex& a_V)
{
    if (_vertice.contains(a_V)) {
        auto vertexI = _vertice[a_V];
        auto vertex  = _Vertex_Merge(_vertice[vertexI], a_V);
        return _vertice.insert(vertex).first;
    } else {
        auto vertexI = _vertice.insert(a_V).first;
        _references.insert({ vertexI, {} });
        return vertexI;
    }
    return -1;
}

void PrimitiveOptimizer::_Vertex_Delete(const uint64_t& a_I)
{
    _references.erase(a_I);
    _vertice.erase(a_I);
}

uint64_t PrimitiveOptimizer::_Pair_Insert(const uint64_t& a_VertexI0, const uint64_t& a_VertexI1)
{
    return _Pair_Insert({ a_VertexI0, a_VertexI1 });
}

uint64_t PrimitiveOptimizer::_Pair_Insert(const Pair& a_Pair)
{
    auto ret = _pairs.insert(a_Pair);
    if (ret.second) {
        auto& pairI = ret.first;
        _references.at(a_Pair.vertice[0]).pairs.insert(pairI);
        _references.at(a_Pair.vertice[1]).pairs.insert(pairI);
        _Pair_Update(pairI); // Compute initial contraction cost
    }
    return ret.first;
}

void PrimitiveOptimizer::_Pair_Delete(const uint64_t& a_PairI)
{
    for (const auto& vertexI : _pairs.at(a_PairI).vertice) {
        _references.at(vertexI).pairs.erase(a_PairI);
    }
    _pairs.erase(a_PairI);
}

void PrimitiveOptimizer::_Pair_Update(const uint64_t& a_PairI)
{
    auto& pair = _pairs.at(a_PairI);
    assert(pair.vertice[0] != pair.vertice[1]);
    const auto& vertI0 = pair.vertice[0];
    const auto& vertI1 = pair.vertice[1];
    const auto& vert0  = _vertice[vertI0];
    const auto& vert1  = _vertice[vertI1];
    const auto& pos0   = vert0.position;
    const auto& pos1   = vert1.position;
    const auto q       = vert0.quadricMatrix + vert1.quadricMatrix;

    constexpr uint8_t sampleCount = 10;
    double minError               = std::numeric_limits<double>::max();
    for (uint8_t i = 0; i <= sampleCount; i++) {
        auto mixValue = i / float(sampleCount);
        auto pos      = glm::mix(pos0, pos1, mixValue);
        auto error    = q.Error(pos);
        if (error < minError) {
            pair.contractionCost = minError = error;
            pair.target                     = _Vertex_Merge(vert0, vert1, mixValue);
        }
    }
}

void PrimitiveOptimizer::_Pair_Sort()
{
    _pairIndice.clear();
    for (const auto& pair : _pairs)
        _pairIndice.push_back(pair.first);
    std::sort(_pairIndice.begin(), _pairIndice.end(), [&pairs = _pairs](auto& a_A, auto& a_B) {
        return pairs[a_A].contractionCost > pairs[a_B].contractionCost;
    });
}

std::shared_ptr<Primitive> PrimitiveOptimizer::_ReconstructPrimitive() const
{
    // Generate new primitive
    auto vertexCount    = _vertice.size();
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

    int32_t positionsOffset    = 0;
    int32_t normalsOffset      = positionsOffset + positionsSize;
    int32_t tangentsOffset     = normalsOffset + normalsSize;
    int32_t texCoords0Offset   = tangentsOffset + tangentsSize;
    int32_t texCoords1Offset   = texCoords0Offset + texCoords0Size;
    int32_t texCoords2Offset   = texCoords1Offset + texCoords1Size;
    int32_t texCoords3Offset   = texCoords2Offset + texCoords2Size;
    int32_t colorsOffset       = texCoords3Offset + texCoords3Size;
    int32_t jointsOffset       = colorsOffset + colorsSize;
    int32_t weightsOffset      = jointsOffset + jointsSize;
    auto totalVertexBufferSize = weightsOffset + weightsSize;

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

    uint64_t vertexIndex = 0;
    std::unordered_map<uint64_t, uint64_t> vertexCorrespondance;
    vertexCorrespondance.reserve(_vertice.size());

    for (const auto& pair : _vertice) {
        const auto& vertexI           = pair.first;
        const auto& vertex            = pair.second;
        vertexCorrespondance[vertexI] = vertexIndex;
        vertexIndex++;
        positionsFinal.push_back(vertex.position);
        if (_hasNormals)
            normalsFinal.push_back(vertex.normal);
        if (_hasTangents)
            tangentsFinal.push_back(vertex.tangent);
        if (_hasTexCoord0)
            texCoords0Final.push_back(vertex.texCoord0);
        if (_hasTexCoord1)
            texCoords1Final.push_back(vertex.texCoord1);
        if (_hasTexCoord2)
            texCoords2Final.push_back(vertex.texCoord2);
        if (_hasTexCoord3)
            texCoords3Final.push_back(vertex.texCoord3);
        if (_hasColors)
            colorsFinal.push_back(vertex.color);
        if (_hasJoints)
            jointsFinal.push_back(vertex.joints);
        if (_hasWeights)
            weightsFinal.push_back(vertex.weights);
    }
    auto vertexBuffer     = std::make_shared<SG::Buffer>(totalVertexBufferSize);
    auto vertexbufferView = std::make_shared<BufferView>(vertexBuffer, 0, vertexBuffer->size());
    std::memcpy(vertexBuffer->data() + positionsOffset, positionsFinal.data(), positionsSize);
    std::memcpy(vertexBuffer->data() + normalsOffset, normalsFinal.data(), normalsSize);
    std::memcpy(vertexBuffer->data() + tangentsOffset, tangentsFinal.data(), tangentsSize);
    std::memcpy(vertexBuffer->data() + texCoords0Offset, texCoords0Final.data(), texCoords0Size);
    std::memcpy(vertexBuffer->data() + texCoords1Offset, texCoords1Final.data(), texCoords1Size);
    std::memcpy(vertexBuffer->data() + texCoords2Offset, texCoords2Final.data(), texCoords2Size);
    std::memcpy(vertexBuffer->data() + texCoords3Offset, texCoords3Final.data(), texCoords3Size);
    std::memcpy(vertexBuffer->data() + colorsOffset, colorsFinal.data(), colorsSize);
    std::memcpy(vertexBuffer->data() + jointsOffset, jointsFinal.data(), jointsSize);
    std::memcpy(vertexBuffer->data() + weightsOffset, weightsFinal.data(), weightsSize);

    auto indexCount      = _triangles.size() * 3;
    auto indexBufferSize = sizeof(uint32_t) * indexCount;
    auto indexBuffer     = std::make_shared<SG::Buffer>();
    auto indexbufferView = std::make_shared<BufferView>(indexBuffer, 0, indexBufferSize);
    indexBuffer->reserve(indexBufferSize);
    for (const auto& pair : _triangles) {
        const auto& triangleI = pair.first;
        const auto& triangle  = pair.second;
        for (const auto& vertexI : triangle.vertice) {
            uint32_t index = vertexCorrespondance.at(vertexI);
            indexBuffer->push_back(index);
        }
    }

    auto newPrimitive = std::make_shared<SG::Primitive>();
    newPrimitive->SetPositions({ vertexbufferView, positionsOffset, vertexCount, DataType::Float32, posType::length() });
    if (_hasNormals)
        newPrimitive->SetNormals({ vertexbufferView, normalsOffset, vertexCount, DataType::Float32, norType::length() });
    if (_hasTangents)
        newPrimitive->SetTangent({ vertexbufferView, tangentsOffset, vertexCount, DataType::Float32, tanType::length() });
    if (_hasTexCoord0)
        newPrimitive->SetTexCoord0({ vertexbufferView, texCoords0Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasTexCoord1)
        newPrimitive->SetTexCoord1({ vertexbufferView, texCoords1Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasTexCoord2)
        newPrimitive->SetTexCoord2({ vertexbufferView, texCoords2Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasTexCoord3)
        newPrimitive->SetTexCoord3({ vertexbufferView, texCoords3Offset, vertexCount, DataType::Float32, texType::length() });
    if (_hasColors)
        newPrimitive->SetColors({ vertexbufferView, colorsOffset, vertexCount, DataType::Float32, colType::length() });
    if (_hasJoints)
        newPrimitive->SetJoints({ vertexbufferView, jointsOffset, vertexCount, DataType::Float32, joiType::length() });
    if (_hasWeights)
        newPrimitive->SetWeights({ vertexbufferView, weightsOffset, vertexCount, DataType::Float32, weiType::length() });
    newPrimitive->SetIndices({ indexbufferView, 0, indexCount, DataType::Uint32, 1 });
    newPrimitive->ComputeBoundingVolume();
    return newPrimitive;
}
}
