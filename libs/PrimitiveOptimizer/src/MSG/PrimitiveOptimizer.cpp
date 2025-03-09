#include <MSG/Debug.hpp>
#include <MSG/Mesh/Primitive.hpp>
#include <MSG/Plane.hpp>
#include <MSG/PrimitiveOptimizer.hpp>
#include <MSG/Tools/ScopedTimer.hpp>

#include <glm/gtx/common.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <numeric>

using namespace MSG::PO;

namespace MSG {
template <typename T>
static T BarycentricCoords(const T& a_Pos, const T& a_V0, const T& a_V1, const T& a_V2)
{
    auto v0    = a_V1 - a_V0;
    auto v1    = a_V2 - a_V0;
    auto v2    = a_Pos - a_V0;
    auto d00   = glm::dot(v0, v0);
    auto d01   = glm::dot(v0, v1);
    auto d11   = glm::dot(v1, v1);
    auto d20   = glm::dot(v2, v0);
    auto d21   = glm::dot(v2, v1);
    auto denom = d00 * d11 - d01 * d01;
    auto v     = (d11 * d20 - d01 * d21) / denom;
    auto w     = (d00 * d21 - d01 * d20) / denom;
    auto u     = 1.0 - v - w;
    return { u, v, w };
}
template <typename T>
static T TriangleNormal(const T& a_P0, const T& a_P1, const T& a_P2)
{
    return glm::normalize(glm::cross(a_P2 - a_P0, a_P1 - a_P0));
}
template <typename T>
static T Project(const T& a_OldValue, const T& a_OldMin, const T& a_OldMax, const T& a_NewMin, const T& a_NewMax)
{
    auto OldRange = (a_OldMax - a_OldMin);
    auto NewRange = (a_NewMax - a_NewMin);
    return (((a_OldValue - a_OldMin) * NewRange) / OldRange) + a_NewMin;
}

template <unsigned L, typename T, bool Normalized = false>
static glm::vec<L, T> ConvertData(const BufferAccessor& a_Accessor, size_t a_Index)
{
    const auto componentNbr = a_Accessor.GetComponentNbr();
    glm::vec<L, T> ret {};
    for (auto i = 0u; i < L && i < componentNbr; ++i) {
        switch (a_Accessor.GetComponentType()) {
        case Core::DataType::Int8:
            ret[i] = T(a_Accessor.template GetComponent<glm::int8>(a_Index, i));
            break;
        case Core::DataType::Uint8:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint8>(a_Index, i));
            break;
        case Core::DataType::Int16:
            ret[i] = T(a_Accessor.template GetComponent<glm::int16>(a_Index, i));
            break;
        case Core::DataType::Uint16:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint16>(a_Index, i));
            break;
        case Core::DataType::Int32:
            ret[i] = T(a_Accessor.template GetComponent<glm::int32>(a_Index, i));
            break;
        case Core::DataType::Uint32:
            ret[i] = T(a_Accessor.template GetComponent<glm::uint32>(a_Index, i));
            break;
        case Core::DataType::Float16:
            ret[i] = T(glm::detail::toFloat32(a_Accessor.template GetComponent<glm::detail::hdata>(a_Index, i)));
            break;
        case Core::DataType::Float32:
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

void PrimitiveOptimizer::_Preserve_Bounds(const uint64_t& a_TriangleI)
{
    _Preserve_Bounds(_triangles.at(a_TriangleI));
}

void PrimitiveOptimizer::_Preserve_Bounds(const PO::Triangle& a_Triangle)
{
    for (uint8_t i = 0; i < 3; i++) {
        auto j   = (i + 1) % 3;
        auto& v0 = a_Triangle.vertice[i];
        auto& v1 = a_Triangle.vertice[j];
        PO::Pair edge(v0, v1);
        if (_pairs.find(edge)->second.edge) {
            auto& vert0  = _vertice.at(edge.vertice[0]);
            auto& vert1  = _vertice.at(edge.vertice[1]);
            auto& pos0   = vert0.position;
            auto& pos1   = vert1.position;
            auto center  = a_Triangle.plane.GetPosition();
            auto normal  = a_Triangle.plane.GetNormal();
            auto edgeDir = glm::normalize(pos0 - pos1);
            auto perp    = glm::perp(edgeDir, normal);
            if (glm::dot(perp, center) < 0)
                perp = -perp;
            auto plane      = Plane(pos0, perp);
            auto quadMatrix = PO::SymetricMatrix(plane[0], plane[1], plane[2], plane[3]);
            quadMatrix *= 1000.f;
            vert0.quadricMatrix += quadMatrix;
            vert1.quadricMatrix += quadMatrix;
        }
    }
}

void PrimitiveOptimizer::_Cleanup()
{
    for (auto itr = _vertice.begin(); itr != _vertice.end();) {
        auto& ref = _references[itr->first];
        if (ref.triangles.empty()) {
            for (auto& pairI : ref.pairs)
                _Pair_Unref(pairI);
            _references.erase(itr->first);
            itr = _vertice.erase(itr);
        } else
            itr++;
    }
}

bool PrimitiveOptimizer::_CheckReferencesValidity() const
{
    for (const auto& vertexP : _vertice) {
        const auto& vertexI     = vertexP.first;
        const bool isReferenced = _references.find(vertexI) != _references.end();
        if (!isReferenced) {
            errorStream << "Vertex " << vertexI << " not referenced.\n";
            return false;
        }
    }
    for (const auto& triangleP : _triangles) {
        const auto& triangleI = triangleP.first;
        const auto& triangle  = triangleP.second;
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
    for (auto& refP : _references) {
        const auto& refI = refP.first;
        const auto& ref  = refP.second;
        if (!_vertice.contains(refI)) {
            errorStream << "Vertex " << refI << " referenced but not in vertice list.\n";
            return false;
        }
        for (const auto& triangleI : ref.triangles) {
            const auto& triangle = _triangles.at(triangleI);
            auto itr             = std::find(triangle.vertice.begin(), triangle.vertice.end(), refI);
            bool valid           = itr != triangle.vertice.end();
            if (!valid) {
                errorStream << "Triangle " << triangleI << " referenced at " << refI << " but does not point to this reference.\n";
                return false;
            }
        }
        for (const auto& pairI : ref.pairs) {
            const auto& pair = _pairs.at(pairI);
            auto itr         = std::find(pair.vertice.begin(), pair.vertice.end(), refI);
            bool valid       = itr != pair.vertice.end();
            if (!valid) {
                errorStream << "Pair " << pairI << " referenced at " << refI << " but does not point to this reference.\n";
                return false;
            }
        }
    }
    return true;
}

PrimitiveOptimizer::PrimitiveOptimizer(const std::shared_ptr<MeshPrimitive>& a_Primitive)
    : _min(a_Primitive->GetBoundingVolume().Min())
    , _max(a_Primitive->GetBoundingVolume().Max())
    , _hasNormals(!a_Primitive->GetNormals().empty())
    , _hasTangents(!a_Primitive->GetTangent().empty())
    , _hasTexCoord0(!a_Primitive->GetTexCoord0().empty())
    , _hasTexCoord1(!a_Primitive->GetTexCoord1().empty())
    , _hasTexCoord2(!a_Primitive->GetTexCoord2().empty())
    , _hasTexCoord3(!a_Primitive->GetTexCoord3().empty())
    , _hasColors(!a_Primitive->GetColors().empty())
    , _hasJoints(!a_Primitive->GetJoints().empty())
    , _hasWeights(!a_Primitive->GetWeights().empty())
{

    if (a_Primitive->GetDrawingMode() != MeshPrimitive::DrawingMode::Triangles) {
        errorLog("Mesh optimization only available for triangulated meshes");
        return;
    }
    _references.set_deleted_key(std::numeric_limits<uint64_t>::max());
    _pairRefCounts.set_deleted_key(std::numeric_limits<uint64_t>::max());
    _vertice.reserve(a_Primitive->GetPositions().GetSize());
    _references.reserve(_vertice.size());
    _pairRefCounts.reserve(_vertice.size() * _vertice.size());

    debugStream << "Loading mesh...\n";
    if (!a_Primitive->GetIndices().empty()) {
        _triangles.reserve(a_Primitive->GetIndices().GetSize() / 3);
        if (a_Primitive->GetIndices().GetComponentType() == Core::DataType::Uint32)
            _FromIndexed(a_Primitive, BufferTypedAccessor<uint32_t>(a_Primitive->GetIndices()));
        else if (a_Primitive->GetIndices().GetComponentType() == Core::DataType::Uint16)
            _FromIndexed(a_Primitive, BufferTypedAccessor<uint16_t>(a_Primitive->GetIndices()));
    } else {
        for (uint32_t i = 0; i < a_Primitive->GetPositions().GetSize(); i += 3)
            _PushTriangle(a_Primitive, { i + 0, i + 1, i + 2 });
    }
    debugStream << "Loading done.\n";
    debugStream << "Vertice count   : " << _vertice.size() << '\n';
    debugStream << "Triangles count : " << _triangles.size() << '\n';
    debugStream << "Adding mesh edges to valid pairs...\n";
    for (const auto& triangleP : _triangles) {
        const auto& triangle = triangleP.second;
        _Triangle_UpdateVertice(triangle); // compute initial contraction cost
        _Preserve_Bounds(triangle);
    }
    for (const auto& pair : _pairs)
        _Pair_Update(pair.second);

    //  Initiate pair indice and sort
    _pairIndice.resize(_pairs.size());
    std::iota(_pairIndice.begin(), _pairIndice.end(), 0);
    _Pair_Sort();
    assert(_CheckReferencesValidity());
}

std::shared_ptr<MeshPrimitive> PrimitiveOptimizer::operator()(const float& a_CompressionRatio, const float& a_MaxCompressionCost)
{
    auto timer                        = Tools::ScopedTimer("Mesh compression");
    const auto compressionRatio       = std::clamp(a_CompressionRatio, 0.f, 1.f);
    const auto targetCompressionRatio = 1 - compressionRatio;
    const auto targetTrianglesCount   = std::max(uint32_t(_triangles.size() * targetCompressionRatio), 3u);
    const auto inputTriangleCount     = _triangles.size();

    google::sparse_hash_set<PO::Triangle> newTriangles(_triangles.size());
    google::sparse_hash_set<uint64_t> updatedVertice(_vertice.size());
    google::sparse_hash_set<uint64_t> pairsToUpdate(_pairs.size());

    debugStream << "Starting mesh compression..." << '\n';
    debugStream << "Wanted compression ratio: " << compressionRatio * 100.f << "%\n";
    debugStream << "Max compression cost    : " << a_MaxCompressionCost << '\n';
    debugStream << "Input triangles count   : " << _triangles.size() << '\n';
    debugStream << "Target triangles count  : " << targetTrianglesCount << '\n';
    // tries is a failsafe in case we fail to collapse triangles too much
    for (uint8_t tries = 0; tries < 100; tries++) {
        if (_triangles.size() <= targetTrianglesCount) {
            debugStream << "Cannot optimize further : target compression reached !\n";
            break;
        }
        const auto& pairToCollapseI   = _pairIndice.back();
        const PO::Pair pairToCollapse = _pairs.at(pairToCollapseI);
        if (pairToCollapse.contractionCost >= a_MaxCompressionCost) {
            debugStream << "Cannot optimize further : max contraction cost reached " << pairToCollapse.contractionCost << "/" << a_MaxCompressionCost << "\n";
            break;
        }
        // Create new vertex if necessary
        auto newVertexI = _Vertex_Insert(pairToCollapse.target);
        PO::Reference refToMerge;
        // Move current pair's vertice to new vertex
        for (auto& vertexI : pairToCollapse.vertice)
            refToMerge << _references[vertexI];
        refToMerge << _references[newVertexI];
        // Create new triangles and pairs if necessary
        for (auto& triangleI : refToMerge.triangles) {
            PO::Triangle newTriangle = _triangles.at(triangleI);
            _Triangle_Delete(triangleI);
            auto& v0         = _vertice[newTriangle.vertice[0]];
            auto& v1         = _vertice[newTriangle.vertice[1]];
            auto& v2         = _vertice[newTriangle.vertice[2]];
            auto barycentric = BarycentricCoords(
                pairToCollapse.target,
                v0.position, v1.position, v2.position);
            auto newAttribs = newTriangle.InterpolateAttribs(barycentric);
            for (auto& oldVertexI : pairToCollapse.vertice) {
                for (uint8_t i = 0; i < 3; i++) {
                    auto& vertexI = newTriangle.vertice[i];
                    if (vertexI == oldVertexI) {
                        auto& attribs = newTriangle.attribs[i];
                        // keep joints and weights until we find a better way to interpolate them
                        newAttribs.joints  = attribs.joints;
                        newAttribs.weights = attribs.weights;
                        vertexI            = newVertexI;
                        attribs            = newAttribs;
                    }
                }
            }
            if (!_Triangle_Update(newTriangle)) {
                tries = 0;
                continue;
            }
            _Triangle_HandleInversion(newTriangle);
            newTriangles.insert(newTriangle);
        }
        for (const auto& pairI : refToMerge.pairs) {
            auto pairItr = _pairs.find(pairI);
            if (pairItr == _pairs.end()) // already deleted
                continue;
            PO::Pair pair = pairItr->second;
            _Pair_Unref(pairI);
            if (pairI == pairToCollapseI)
                continue;
            for (const auto& oldVertexI : pairToCollapse.vertice)
                std::replace(pair.vertice.begin(), pair.vertice.end(), oldVertexI, newVertexI);
            if (pair.vertice[0] != pair.vertice[1])
                _Pair_Ref(pair);
        }

        // Update quadric matrice and pairs contraction costs
        for (auto& triangle : newTriangles) {
            auto index = _Triangle_Insert(triangle);
            for (uint8_t i = 0; i < 3; i++) {
                auto j    = (i + 1) % 3;
                auto& vI0 = triangle.vertice[i];
                auto& vI1 = triangle.vertice[j];
                if (updatedVertice.find(vI0) != updatedVertice.end()) {
                    updatedVertice.insert(vI0);
                    _vertice.at(vI0).quadricMatrix = {};
                    for (auto& pairI : _references[vI0].pairs)
                        pairsToUpdate.insert(pairI);
                }
                pairsToUpdate.insert(_pairs.at(PO::Pair(vI0, vI1)));
            }
            _Triangle_UpdateVertice(index);
            _Preserve_Bounds(index);
        }
        newTriangles.clear();
        updatedVertice.clear();
        for (auto& pairI : pairsToUpdate)
            _Pair_Update(pairI);
        pairsToUpdate.clear();
        _pairIndice.pop_back();
        _Pair_Sort();
    }
    _Cleanup();
    debugStream << "Output triangle count   : " << _triangles.size() << '\n';
    consoleStream << "Compression ratio       : " << (1 - (_triangles.size() / float(inputTriangleCount))) * 100.f << "%\n";
    resultCompressionRatio = 1 - (_triangles.size() / float(inputTriangleCount));
    return result          = _ReconstructPrimitive();
}

template <typename Accessor>
inline void PrimitiveOptimizer::_FromIndexed(const std::shared_ptr<MeshPrimitive>& a_Primitive, const Accessor& a_Indice)
{
    for (uint32_t i = 0; i < a_Indice.GetSize(); i += 3)
        _PushTriangle(a_Primitive, { a_Indice.at(i + 0), a_Indice.at(i + 1), a_Indice.at(i + 2) });
}

void PrimitiveOptimizer::_PushTriangle(const std::shared_ptr<MeshPrimitive>& a_Primitive, const std::array<uint32_t, 3>& a_Indice)
{
    PO::Triangle triangle = {};
    for (uint32_t i = 0; i < 3; i++) {
        PO::Vertex vertex             = {};
        vertex                        = ConvertData<posType::length(), posType::value_type>(a_Primitive->GetPositions(), a_Indice[i]);
        vertex                        = Project(vertex.position, _min, _max, posType(0), posType(1));
        triangle.vertice[i]           = _Vertex_Insert(vertex);
        triangle.attribs[i].normal    = _hasNormals ? ConvertData<norType::length(), norType::value_type, true>(a_Primitive->GetNormals(), a_Indice[i]) : norType {};
        triangle.attribs[i].tangent   = _hasTangents ? ConvertData<tanType::length(), tanType::value_type>(a_Primitive->GetTangent(), a_Indice[i]) : tanType {};
        triangle.attribs[i].texCoord0 = _hasTexCoord0 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord0(), a_Indice[i]) : texType {};
        triangle.attribs[i].texCoord1 = _hasTexCoord1 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord1(), a_Indice[i]) : texType {};
        triangle.attribs[i].texCoord2 = _hasTexCoord2 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord2(), a_Indice[i]) : texType {};
        triangle.attribs[i].texCoord3 = _hasTexCoord3 ? ConvertData<texType::length(), texType::value_type>(a_Primitive->GetTexCoord3(), a_Indice[i]) : texType {};
        triangle.attribs[i].color     = _hasColors ? ConvertData<colType::length(), colType::value_type>(a_Primitive->GetColors(), a_Indice[i]) : colType {};
        triangle.attribs[i].joints    = _hasJoints ? ConvertData<joiType::length(), joiType::value_type>(a_Primitive->GetJoints(), a_Indice[i]) : joiType {};
        triangle.attribs[i].weights   = _hasWeights ? ConvertData<weiType::length(), weiType::value_type>(a_Primitive->GetWeights(), a_Indice[i]) : weiType {};
    };
    // Don't keep already collapsed triangles
    if (!_Triangle_Update(triangle))
        return;
    triangle.originalNormal = triangle.plane.GetNormal();
    _Triangle_Insert(triangle);
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const uint64_t& a_TriangleI) const
{
    return _Triangle_IsCollapsed(_triangles.at(a_TriangleI));
}

bool PrimitiveOptimizer::_Triangle_IsCollapsed(const PO::Triangle& a_Triangle) const
{
    const auto& posI0 = _vertice.at(a_Triangle.vertice[0]);
    const auto& posI1 = _vertice.at(a_Triangle.vertice[1]);
    const auto& posI2 = _vertice.at(a_Triangle.vertice[2]);
    return posI0 == posI1
        || posI0 == posI2
        || posI1 == posI2;
}

void PrimitiveOptimizer::_Triangle_Delete(const uint64_t& a_TriangleI)
{
    auto& triangle = _triangles.at(a_TriangleI);
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t j = (i + 1) % 3;
        auto& vI0 = triangle.vertice[i];
        auto& vI1 = triangle.vertice[j];
        _references[vI0].triangles.erase(a_TriangleI);
        _Pair_Unref(PO::Pair(vI0, vI1));
    }
    _triangles.erase(a_TriangleI);
}

bool PrimitiveOptimizer::_Triangle_Update(const uint64_t& a_TriangleI)
{
    return _Triangle_Update(_triangles[a_TriangleI]);
}

bool PrimitiveOptimizer::_Triangle_Update(const PO::Triangle& a_Triangle) const
{
    if ((a_Triangle.collapsed = _Triangle_IsCollapsed(a_Triangle)))
        return false;
    const auto& position0    = _vertice.at(a_Triangle.vertice[0]).position;
    const auto& position1    = _vertice.at(a_Triangle.vertice[1]).position;
    const auto& position2    = _vertice.at(a_Triangle.vertice[2]).position;
    const auto normal        = TriangleNormal(position0, position1, position2);
    a_Triangle.plane         = Plane(position0, normal);
    a_Triangle.quadricMatrix = PO::SymetricMatrix(a_Triangle.plane[0], a_Triangle.plane[1], a_Triangle.plane[2], a_Triangle.plane[3]);
    return true;
}

uint64_t PrimitiveOptimizer::_Triangle_Insert(const PO::Triangle& a_Triangle)
{
    auto triangleI = _triangles.insert(a_Triangle).first;
    for (uint8_t i = 0; i < 3; i++) {
        uint8_t j = (i + 1) % 3;
        auto& vI0 = a_Triangle.vertice[i];
        auto& vI1 = a_Triangle.vertice[j];
        auto& ref = _references.find(vI0)->second;
        ref.triangles.insert(triangleI);
        _Pair_Ref(vI0, vI1);
    }
    return triangleI;
}

void PrimitiveOptimizer::_Triangle_HandleInversion(PO::Triangle& a_Triangle) const
{
    if (glm::dot(a_Triangle.plane.GetNormal(), a_Triangle.originalNormal) < 0) {
        std::swap(a_Triangle.vertice[0], a_Triangle.vertice[2]);
        std::swap(a_Triangle.attribs[0], a_Triangle.attribs[2]);
        _Triangle_Update(a_Triangle);
    }
}

void PrimitiveOptimizer::_Triangle_UpdateVertice(const uint64_t& a_TriangleI)
{
    return _Triangle_UpdateVertice(_triangles.at(a_TriangleI));
}

void PrimitiveOptimizer::_Triangle_UpdateVertice(const PO::Triangle& a_Triangle)
{
    for (uint8_t i = 0; i < 3; i++) {
        const auto& vertexI = a_Triangle.vertice[i];
        const auto& vertex  = _vertice.at(vertexI);
        vertex.quadricMatrix += a_Triangle.quadricMatrix;
    }
}

PO::Vertex PrimitiveOptimizer::_Vertex_Merge(const uint64_t& a_I0, const uint64_t& a_I1, const float& a_X)
{
    return _Vertex_Merge(_vertice[a_I0], _vertice[a_I1], a_X);
}

PO::Vertex PrimitiveOptimizer::_Vertex_Merge(const PO::Vertex& a_V0, const PO::Vertex& a_V1, const float& a_X)
{
    return glm::mix(a_V0.position, a_V1.position, a_X);
}

uint64_t PrimitiveOptimizer::_Vertex_Insert(const PO::Vertex& a_V)
{
    if (auto itr = _vertice.find(a_V); itr != _vertice.end()) {
        return itr->first;
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

uint64_t PrimitiveOptimizer::_Pair_Ref(const uint64_t& a_VertexI0, const uint64_t& a_VertexI1)
{
    return _Pair_Ref({ a_VertexI0, a_VertexI1 });
}

uint64_t PrimitiveOptimizer::_Pair_Ref(const PO::Pair& a_Pair)
{
    auto ret = _pairs.insert(a_Pair);
    if (ret.second) {
        auto& pairI = ret.first;
        auto& ref0  = _references[a_Pair.vertice[0]];
        auto& ref1  = _references[a_Pair.vertice[1]];
        assert(_pairRefCounts.find(pairI) == _pairRefCounts.end());
        ref0.pairs.insert(pairI);
        ref1.pairs.insert(pairI);
        _pairRefCounts.insert({ pairI, 1 });
        assert(_pairRefCounts[pairI] == 1);
        _pairs.at(ret.first).edge = true;
    } else {
        auto& refCount = _pairRefCounts[ret.first];
        refCount++;
        _pairs.at(ret.first).edge = refCount == 1;
    }
    return ret.first;
}

void PrimitiveOptimizer::_Pair_Unref(const uint64_t& a_PairI)
{
    auto itr = _pairs.find(a_PairI);
    if (itr == _pairs.end()) {
        assert(_pairRefCounts.find(a_PairI) == _pairRefCounts.end());
        return;
    }
    auto refCountItr = _pairRefCounts.find(a_PairI);
    assert(refCountItr != _pairRefCounts.end());
    auto& refCount = refCountItr->second;
    refCount--;
    if (refCount == 0) {
        for (const auto& vertexI : itr->second.vertice)
            _references[vertexI].pairs.erase(itr->first);
        _pairRefCounts.erase(refCountItr);
        _pairs.erase(itr);
    } else {
        itr->second.edge = refCount == 1;
    }
}

void PrimitiveOptimizer::_Pair_Unref(const PO::Pair& a_Pair)
{
    if (auto itr = _pairs.find(a_Pair); itr != _pairs.end())
        _Pair_Unref(itr->first);
}

void PrimitiveOptimizer::_Pair_Update(const uint64_t& a_PairI)
{
    assert(_pairRefCounts.find(a_PairI) != _pairRefCounts.end());
    _Pair_Update(_pairs.at(a_PairI));
}

// 0 - 0.25 - 0.5 - 0.75 - 1  sampleCount = 5  sampleSpace = 1/4
// 0 --- 0.33 --- 0.66 --- 1  sampleCount = 4  sampleSpace = 1/3
// 0 -------- 0.5 -------- 1  sampleCount = 3  sampleSpace = 1/2
// 0 --------------------- 1  sampleCount = 2  sampleSpace = 1/1
// ---------- 0.5 ----------  sampleCount = 1  sampleSpace = ???
void PrimitiveOptimizer::_Pair_Update(const PO::Pair& a_Pair)
{
    assert(a_Pair.vertice[0] != a_Pair.vertice[1]);
    constexpr uint32_t sampleCount = 5u;
    constexpr float sampleSpace    = sampleCount > 1 ? 1.f / (sampleCount - 1) : 0;
    const auto& vert0              = _vertice[a_Pair.vertice[0]];
    const auto& vert1              = _vertice[a_Pair.vertice[1]];
    const auto& position0          = _vertice[a_Pair.vertice[0]].position;
    const auto& position1          = _vertice[a_Pair.vertice[1]].position;
    const auto q                   = vert0.quadricMatrix + vert1.quadricMatrix;
    if constexpr (sampleCount == 1) {
        a_Pair.target          = glm::mix(position0, position1, 0.5f);
        a_Pair.contractionCost = q.Error(a_Pair.target);
        return;
    }
    a_Pair.contractionCost = std::numeric_limits<double>::max();
    for (uint32_t i = 0u; i < sampleCount; i++) {
        const float mixValue = i * sampleSpace;
        const auto targetPos = glm::mix(position0, position1, mixValue);
        const double error   = q.Error(targetPos);
        if (error < a_Pair.contractionCost) {
            a_Pair.target          = targetPos;
            a_Pair.contractionCost = error;
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

std::shared_ptr<MeshPrimitive> PrimitiveOptimizer::_ReconstructPrimitive() const
{
    auto vertexCount = _triangles.size() * 3;
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
        for (auto& vertexI : triangle.second.vertice) {
            auto& vertex = _vertice.at(vertexI);
            positionsFinal.push_back(Project(vertex.position, posType(0), posType(1), _min, _max));
        }
        for (auto& vertexData : triangle.second.attribs) {
            if (_hasNormals)
                normalsFinal.push_back(vertexData.normal);
            if (_hasTangents)
                tangentsFinal.push_back(vertexData.tangent);
            if (_hasTexCoord0)
                texCoords0Final.push_back(vertexData.texCoord0);
            if (_hasTexCoord1)
                texCoords1Final.push_back(vertexData.texCoord1);
            if (_hasTexCoord2)
                texCoords2Final.push_back(vertexData.texCoord2);
            if (_hasTexCoord3)
                texCoords3Final.push_back(vertexData.texCoord3);
            if (_hasColors)
                colorsFinal.push_back(vertexData.color);
            if (_hasJoints)
                jointsFinal.push_back(vertexData.joints);
            if (_hasWeights)
                weightsFinal.push_back(vertexData.weights);
        }
    }

    // Generate new primitive
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

    auto vertexBuffer     = std::make_shared<Buffer>(totalVertexBufferSize);
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

    auto newPrimitive = std::make_shared<MeshPrimitive>();
    newPrimitive->SetPositions({ vertexbufferView, positionsOffset, vertexCount, Core::DataType::Float32, posType::length() });
    if (_hasNormals)
        newPrimitive->SetNormals({ vertexbufferView, normalsOffset, vertexCount, Core::DataType::Float32, norType::length() });
    if (_hasTangents)
        newPrimitive->SetTangent({ vertexbufferView, tangentsOffset, vertexCount, Core::DataType::Float32, tanType::length() });
    if (_hasTexCoord0)
        newPrimitive->SetTexCoord0({ vertexbufferView, texCoords0Offset, vertexCount, Core::DataType::Float32, texType::length() });
    if (_hasTexCoord1)
        newPrimitive->SetTexCoord1({ vertexbufferView, texCoords1Offset, vertexCount, Core::DataType::Float32, texType::length() });
    if (_hasTexCoord2)
        newPrimitive->SetTexCoord2({ vertexbufferView, texCoords2Offset, vertexCount, Core::DataType::Float32, texType::length() });
    if (_hasTexCoord3)
        newPrimitive->SetTexCoord3({ vertexbufferView, texCoords3Offset, vertexCount, Core::DataType::Float32, texType::length() });
    if (_hasColors)
        newPrimitive->SetColors({ vertexbufferView, colorsOffset, vertexCount, Core::DataType::Float32, colType::length() });
    if (_hasJoints)
        newPrimitive->SetJoints({ vertexbufferView, jointsOffset, vertexCount, Core::DataType::Float32, joiType::length() });
    if (_hasWeights)
        newPrimitive->SetWeights({ vertexbufferView, weightsOffset, vertexCount, Core::DataType::Float32, weiType::length() });
    newPrimitive->ComputeBoundingVolume();
    return newPrimitive;
}
}
