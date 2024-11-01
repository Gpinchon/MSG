#include <SG/Core/Primitive.hpp>
#include <SG/PrimitiveOptimizer.hpp>
#include <Tools/Debug.hpp>
#include <Tools/ScopedTimer.hpp>

#include <algorithm>
#include <numeric>

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

bool PrimitiveOptimizer::_CheckReferencesValidity() const
{
    for (const auto& pair : _vertice) {
        const auto& vertexI     = pair.first;
        const bool isReferenced = _references.find(vertexI) != _references.end();
        if (!isReferenced) {
            errorStream << "Vertex " << vertexI << " not referenced.\n";
            return false;
        }
    }
    for (const auto& pair : _triangles) {
        const auto& triangleI = pair.first;
        const auto& triangle  = pair.second;
        for (const auto& vertexI : triangle.vertice) {
            auto& ref               = _references.find(vertexI)->second;
            const bool isReferenced = ref.ContainsTriangle(triangleI);
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
            auto& ref               = _references.find(vertexI)->second;
            const bool isReferenced = ref.ContainsPair(pairI);
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
    _references.set_deleted_key(std::numeric_limits<uint64_t>::max());
    if (a_Primitive->GetDrawingMode() != SG::Primitive::DrawingMode::Triangles) {
        errorLog("Mesh optimization only available for triangulated meshes");
        return;
    }

    _vertice.reserve(a_Primitive->GetPositions().GetSize());
    _references.reserve(_vertice.size());

    debugStream << "Loading mesh...\n";
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
    debugStream << "Loading done.\n";
    debugStream << "Vertice count   : " << _vertice.size() << '\n';
    debugStream << "Triangles count : " << _triangles.size() << '\n';
    debugStream << "Adding mesh edges to valid pairs...\n";
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
    debugStream << "Adding close vertice to valid pairs, distance threshold : " << a_DistanceThreshold << '\n';
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
    auto timer                        = Tools::ScopedTimer("Mesh compression");
    const auto compressionRatio       = std::clamp(a_CompressionRatio, 0.f, 1.f);
    const auto targetCompressionRatio = 1 - compressionRatio;
    const auto targetTrianglesCount   = std::max(uint32_t(_triangles.size() * targetCompressionRatio), 3u);
    const auto inputTriangleCount     = _triangles.size();
    auto currentTrianglesCount        = _triangles.size();
    debugStream << "Starting mesh compression..." << '\n';
    debugStream << "Wanted compression ratio: " << compressionRatio * 100.f << "%\n";
    debugStream << "Max compression cost    : " << a_MaxCompressionCost << '\n';
    debugStream << "Input triangles count   : " << currentTrianglesCount << '\n';
    debugStream << "Target triangles count  : " << targetTrianglesCount << '\n';
    while (currentTrianglesCount > targetTrianglesCount) {
        const auto& pairToCollapseI = _pairIndice.back();
        const POPair pairToCollapse = _pairs[pairToCollapseI];

        if (pairToCollapse.contractionCost > a_MaxCompressionCost) {
            debugStream << "Cannot optimize further : max contraction cost reached " << pairToCollapse.contractionCost << "/" << a_MaxCompressionCost << "\n";
            break;
        }
        POReference refToMerge;
        for (uint8_t i = 0; i < 2; i++) {
            const auto& vertexI = pairToCollapse.vertice[i];
            refToMerge << _references.find(vertexI)->second;
        }
        auto newVertexI = _Vertex_Insert(pairToCollapse.target);
        auto& newRef    = _references.find(newVertexI)->second;
        auto& newVertex = _vertice.at(newVertexI);
        refToMerge << newRef;
        for (auto& triangleI : refToMerge.triangles) {
            POTriangle triangle = _triangles.at(triangleI);
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
            POPair pair = _pairs[pairI];
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
    debugStream << "Output triangle count   : " << _triangles.size() << '\n';
    debugStream << "Compression ratio       : " << (1 - (_triangles.size() / float(inputTriangleCount))) * 100.f << "%\n";
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
    POTriangle triangle = {};
    for (uint32_t i = 0; i < 3; i++) {
        POVertex vertex     = {};
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
        auto& ref = _references.find(vertexI)->second;
        ref.triangles.insert(triangleI);
    }
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const uint64_t& a_TriangleI) const
{
    return _Triangle_IsCollapsed(_triangles.at(a_TriangleI));
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const POTriangle& a_Triangle) const
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
    POTriangle triangle = _triangles[a_TriangleI];
    _Triangle_Update(triangle);
}

void PrimitiveOptimizer::_Triangle_Update(POTriangle& a_Triangle) const
{
    const auto& v0           = _vertice.at(a_Triangle.vertice[0]);
    const auto& v1           = _vertice.at(a_Triangle.vertice[1]);
    const auto& v2           = _vertice.at(a_Triangle.vertice[2]);
    const auto& p0           = v0.position;
    const auto& p1           = v1.position;
    const auto& p2           = v2.position;
    a_Triangle.plane         = Component::Plane(p0, glm::normalize(glm::cross(p1 - p0, p2 - p0)));
    a_Triangle.quadricMatrix = POSymetricMatrix(a_Triangle.plane[0], a_Triangle.plane[1], a_Triangle.plane[2], a_Triangle.plane[3]);
    a_Triangle.collapsed     = _Triangle_IsCollapsed(a_Triangle);
}

uint64_t PrimitiveOptimizer::_Triangle_Insert(const POTriangle& a_Triangle)
{
    auto triangleI = _triangles.insert(a_Triangle);
    for (const auto& vertexI : a_Triangle.vertice) {
        auto& ref = _references.find(vertexI)->second;
        ref.triangles.insert(triangleI.first);
    }
    return triangleI.first;
}

void PrimitiveOptimizer::_Triangle_HandleInversion(POTriangle& a_Triangle) const
{
    if (glm::dot(a_Triangle.plane.GetNormal(), a_Triangle.originalNormal) < 0) {
        std::swap(a_Triangle.vertice[0], a_Triangle.vertice[2]);
        _Triangle_Update(a_Triangle);
    }
}

POVertex PrimitiveOptimizer::_Vertex_Merge(const uint64_t& a_I0, const uint64_t& a_I1, const float& a_X)
{
    return _Vertex_Merge(_vertice[a_I0], _vertice[a_I1], a_X);
}

POVertex PrimitiveOptimizer::_Vertex_Merge(const POVertex& a_V0, const POVertex& a_V1, const float& a_X)
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

uint64_t PrimitiveOptimizer::_Vertex_Insert(const POVertex& a_V)
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

uint64_t PrimitiveOptimizer::_Pair_Insert(const POPair& a_Pair)
{
    auto ret = _pairs.insert(a_Pair);
    if (ret.second) {
        auto& pairI = ret.first;
        auto& ref0  = _references.find(a_Pair.vertice[0])->second;
        auto& ref1  = _references.find(a_Pair.vertice[1])->second;
        ref0.pairs.insert(pairI);
        ref1.pairs.insert(pairI);
        _Pair_Update(pairI); // Compute initial contraction cost
    }
    return ret.first;
}

void PrimitiveOptimizer::_Pair_Delete(const uint64_t& a_PairI)
{
    for (const auto& vertexI : _pairs.at(a_PairI).vertice) {
        auto& ref = _references.find(vertexI)->second;
        ref.pairs.erase(a_PairI);
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
