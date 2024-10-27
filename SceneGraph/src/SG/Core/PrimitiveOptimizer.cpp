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

using posType = decltype(PrimitiveOptimizer::positions)::value_type;
using norType = decltype(PrimitiveOptimizer::normals)::value_type;
using tanType = decltype(PrimitiveOptimizer::tangents)::value_type;
using texType = decltype(PrimitiveOptimizer::texCoords)::value_type;
using colType = decltype(PrimitiveOptimizer::colors)::value_type;
using joiType = decltype(PrimitiveOptimizer::joints)::value_type;
using weiType = decltype(PrimitiveOptimizer::weights)::value_type;

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

static auto ComputeError(const SymetricMatrix& a_Q, const glm::vec4& a_V)
{
    return (a_Q[0] * a_V.x * a_V.x) + (2 * a_Q[1] * a_V.x * a_V.y) + (2 * a_Q[2] * a_V.x * a_V.z) + (2 * a_Q[3] * a_V.x)
        + (a_Q[4] * a_V.y * a_V.y) + (2 * a_Q[5] * a_V.y * a_V.z) + (2 * a_Q[6] * a_V.y)
        + (a_Q[7] * a_V.z * a_V.z) + (2 * a_Q[8] * a_V.z)
        + (a_Q[9]);
}

static auto ComputeQuadricMatrix(const Triangle& a_T)
{
    return SymetricMatrix(a_T.plane[0], a_T.plane[1], a_T.plane[2], a_T.plane[3]);
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

void PrimitiveOptimizer::_CheckValidity()
{
    for (uint64_t vertexI = 0; vertexI < vertice.size(); vertexI++) {
        const auto& vertex    = vertice[vertexI];
        const auto& positionI = vertex.position;
        assert(references[positionI].vertice.contains(vertexI));
    }
    for (uint64_t triangleI = 0; triangleI < triangles.size(); triangleI++) {
        const auto& triangle = triangles[triangleI];
        for (const auto& vertexI : triangle.vertice) {
            const auto& vertex    = vertice[vertexI];
            const auto& positionI = vertex.position;
            if (triangle.collapsed) // this triangle shouldn't be referenced anymore
                assert(!references[positionI].triangles.contains(triangleI));
            else
                assert(references[positionI].triangles.contains(triangleI));
        }
    }
    for (const auto& pairI : pairIndice) {
        const auto& pair = pairs[pairI];
        for (const auto& positionI : pair.positions)
            assert(references[positionI].pairs.contains(pairI));
    }
    for (uint64_t refI = 0; refI < references.size(); refI++) {
        const auto& ref = references[refI];
        for (const auto& vertexI : ref.vertice) {
            const auto& vertex    = vertice[vertexI];
            const auto& positionI = vertex.position;
            assert(positionI == refI);
        }
        for (const auto& triangleI : ref.triangles) {
            bool valid           = false;
            const auto& triangle = triangles[triangleI];
            for (const auto& vertexI : triangle.vertice) {
                const auto& vertex    = vertice[vertexI];
                const auto& positionI = vertex.position;
                if (positionI == refI)
                    valid = true;
            }
            assert(valid);
        }
        for (const auto& pairI : ref.pairs) {
            const auto& pair = pairs[pairI];
            bool valid       = false;
            for (auto& positionI : pair.positions) {
                if (positionI == refI)
                    valid = true;
            }
            assert(valid);
        }
    }
}

std::shared_ptr<Primitive> PrimitiveOptimizer::_ReconstructPrimitive() const
{
    // Generate new primitive
    auto vertexCount    = triangles.size() * 3;
    auto positionsSize  = vertexCount * sizeof(posType);
    auto normalsSize    = hasNormals ? vertexCount * sizeof(norType) : 0u;
    auto tangentsSize   = hasTangents ? vertexCount * sizeof(tanType) : 0u;
    auto texCoords0Size = hasTexCoord0 ? vertexCount * sizeof(texType) : 0u;
    auto texCoords1Size = hasTexCoord1 ? vertexCount * sizeof(texType) : 0u;
    auto texCoords2Size = hasTexCoord2 ? vertexCount * sizeof(texType) : 0u;
    auto texCoords3Size = hasTexCoord3 ? vertexCount * sizeof(texType) : 0u;
    auto colorsSize     = hasColors ? vertexCount * sizeof(colType) : 0u;
    auto jointsSize     = hasJoints ? vertexCount * sizeof(joiType) : 0u;
    auto weightsSize    = hasWeights ? vertexCount * sizeof(weiType) : 0u;

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

    for (const auto& triangle : triangles) {
        for (const auto& vertexI : triangle.vertice) {
            const auto& vertex = vertice[vertexI];
            positionsFinal.push_back(positions[vertex.position]);
            if (hasNormals)
                normalsFinal.push_back(normals[vertex.normal]);
            if (hasTangents)
                tangentsFinal.push_back(tangents[vertex.tangent]);
            if (hasTexCoord0)
                texCoords0Final.push_back(texCoords[vertex.texCoord0]);
            if (hasTexCoord1)
                texCoords1Final.push_back(texCoords[vertex.texCoord1]);
            if (hasTexCoord2)
                texCoords2Final.push_back(texCoords[vertex.texCoord2]);
            if (hasTexCoord3)
                texCoords3Final.push_back(texCoords[vertex.texCoord3]);
            if (hasColors)
                colorsFinal.push_back(colors[vertex.color]);
            if (hasJoints)
                jointsFinal.push_back(joints[vertex.joints]);
            if (hasWeights)
                weightsFinal.push_back(weights[vertex.weights]);
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
    if (hasNormals)
        newPrimitive->SetNormals({ bufferView, normalsOffset, vertexCount, DataType::Float32, norType::length() });
    if (hasTangents)
        newPrimitive->SetTangent({ bufferView, tangentsOffset, vertexCount, DataType::Float32, tanType::length() });
    if (hasTexCoord0)
        newPrimitive->SetTexCoord0({ bufferView, texCoords0Offset, vertexCount, DataType::Float32, texType::length() });
    if (hasTexCoord1)
        newPrimitive->SetTexCoord1({ bufferView, texCoords1Offset, vertexCount, DataType::Float32, texType::length() });
    if (hasTexCoord2)
        newPrimitive->SetTexCoord2({ bufferView, texCoords2Offset, vertexCount, DataType::Float32, texType::length() });
    if (hasTexCoord3)
        newPrimitive->SetTexCoord3({ bufferView, texCoords3Offset, vertexCount, DataType::Float32, texType::length() });
    if (hasColors)
        newPrimitive->SetColors({ bufferView, colorsOffset, vertexCount, DataType::Float32, colType::length() });
    if (hasJoints)
        newPrimitive->SetJoints({ bufferView, jointsOffset, vertexCount, DataType::Float32, joiType::length() });
    if (hasWeights)
        newPrimitive->SetWeights({ bufferView, weightsOffset, vertexCount, DataType::Float32, weiType::length() });
    newPrimitive->ComputeBoundingVolume();
    return newPrimitive;
}

PrimitiveOptimizer::PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive, const float& a_Threshold)
    : primitive(a_Primitive)
{
    if (primitive->GetDrawingMode() != SG::Primitive::DrawingMode::Triangles) {
        errorLog("Mesh optimization only available for triangulated meshes");
        return;
    }
    positions.reserve(primitive->GetPositions().GetSize());
    references.reserve(positions.size());
    normals.reserve(primitive->GetNormals().GetSize());
    tangents.reserve(primitive->GetTangent().GetSize());
    texCoords.reserve(primitive->GetTexCoord0().GetSize());
    colors.reserve(primitive->GetColors().GetSize());
    joints.reserve(primitive->GetJoints().GetSize());
    weights.reserve(primitive->GetWeights().GetSize());

    if (!primitive->GetIndices().empty()) {
        triangles.reserve(primitive->GetIndices().GetSize() / 3);
        if (primitive->GetIndices().GetComponentType() == SG::DataType::Uint32)
            _FromIndexed(TypedBufferAccessor<uint32_t>(primitive->GetIndices()));
        else if (primitive->GetIndices().GetComponentType() == SG::DataType::Uint16)
            _FromIndexed(TypedBufferAccessor<uint16_t>(primitive->GetIndices()));
    } else {
        for (uint32_t i = 0; i < primitive->GetPositions().GetSize(); i += 3)
            _PushTriangle({ i + 0, i + 1, i + 2 });
    }
    consoleStream << "Adding mesh edges to valid pairs\n";
    for (uint64_t triangleI = 0; triangleI < triangles.size(); triangleI++) {
        const auto& triangle = triangles[triangleI];

        // Add edges to valid pairs
        const auto& v0 = vertice[triangle.vertice[0]];
        const auto& v1 = vertice[triangle.vertice[1]];
        const auto& v2 = vertice[triangle.vertice[2]];
        auto pairI0    = _InsertPair(Pair(v0.position, v1.position));
        auto pairI1    = _InsertPair(Pair(v1.position, v2.position));
        auto pairI2    = _InsertPair(Pair(v2.position, v0.position));

        // Compute Q matrices
        for (const auto& vertexI : triangle.vertice) {
            const auto& positionI = vertice[vertexI].position;
            references[positionI].quadricMatrix += triangle.quadricMatrix;
        }
    }
    consoleStream << "Adding close vertice to valid pairs, distance threshold : " << a_Threshold << '\n';
    // Add close positions to valid pairs
    for (uint64_t position0I = 0; position0I < positions.size(); ++position0I) {
        auto& position0 = positions[position0I];
        for (uint64_t position1I = 0; position1I < positions.size(); ++position1I) {
            auto& position1 = positions[position1I];
            if (glm::distance(position0, position1) < a_Threshold)
                _InsertPair(Pair(position0I, position1I));
        }
    }
    // Initiate pair indice and sort
    pairIndice.resize(pairs.size());
    std::iota(pairIndice.begin(), pairIndice.end(), 0);
    _SortPairs();
#ifndef NDEBUG
    _CheckValidity();
#endif
}

VertexData PrimitiveOptimizer::_ComputeTarget(const Pair& a_Pair, const uint64_t& a_VertexI0, const uint64_t& a_VertexI1)
{
    const auto& v0 = vertice[a_VertexI0];
    const auto& v1 = vertice[a_VertexI1];
    if (a_Pair.target == 0)
        return _GetVertexData(v0);
    if (a_Pair.target == 1)
        return _GetVertexData(v1);
    if (a_Pair.target == 2)
        return _MergeVertice(v0, v1);
    return {};
}

std::shared_ptr<Primitive> PrimitiveOptimizer::operator()(const float& a_Aggressivity, const float& a_MaxError)
{
    const auto targetCompressionRatio = (1 - std::clamp(a_Aggressivity, 0.f, 1.f));
    const auto targetTrianglesCount   = std::max(uint32_t(triangles.size() * targetCompressionRatio), 3u);
    auto currentTrianglesCount        = triangles.size();
    std::unordered_set<uint64_t> verticeToTransfer;
    std::unordered_set<uint64_t> trianglesToTransfer;
    std::unordered_set<uint64_t> pairsToTransfer;
    verticeToTransfer.reserve(1024);
    trianglesToTransfer.reserve(1024);
    pairsToTransfer.reserve(1024);
    while (currentTrianglesCount > targetTrianglesCount) {
        const auto& pairToCollapseI = pairIndice.back();
        const auto& pairToCollapse  = pairs[pairToCollapseI];
        pairIndice.pop_back();
        if (pairToCollapse.error > a_MaxError) {
            consoleStream << "Cannot optimize further : Max error reached " << pairToCollapse.error << "/" << a_MaxError << "\n";
            break;
        }
        uint64_t newPositionI = -1;
        if (pairToCollapse.target == 0)
            newPositionI = pairToCollapse.positions[0];
        else if (pairToCollapse.target == 1)
            newPositionI = pairToCollapse.positions[1];
        else if (pairToCollapse.target == 2) {
            const auto& position0 = positions[pairToCollapse.positions[0]];
            const auto& position1 = positions[pairToCollapse.positions[1]];
            newPositionI          = _InsertPosition((position0 + position1) * 0.5f);
        }
        auto& newRef = references[newPositionI];
        for (const auto& oldPositionI : pairToCollapse.positions) {
            auto& oldRef = references[oldPositionI];
            verticeToTransfer.insert(oldRef.vertice.begin(), oldRef.vertice.end());
            trianglesToTransfer.insert(oldRef.triangles.begin(), oldRef.triangles.end());
            pairsToTransfer.insert(oldRef.pairs.begin(), oldRef.pairs.end());
            oldRef.vertice.clear();
            oldRef.triangles.clear();
            oldRef.pairs.clear();
            oldRef.quadricMatrix = {};
        }
        for (const auto& vertexI : verticeToTransfer) {
            auto& vertex    = vertice[vertexI];
            vertex.position = newPositionI;
            newRef.vertice.insert(vertexI);
        }
        for (const auto& triangleI : trianglesToTransfer) {
            auto& triangle = triangles[triangleI];
            _UpdateTriangle(triangle);
            if (triangle.collapsed) {
                for (const auto& vertexI : triangle.vertice) {
                    const auto& vertex = vertice[vertexI];
                    references[vertex.position].triangles.erase(triangleI);
                }
                currentTrianglesCount--;
            } else {
                if (glm::dot(triangle.plane.GetNormal(), triangle.originalNormal) < 0) {
                    std::swap(triangle.vertice[0], triangle.vertice[2]);
                    _UpdateTriangle(triangle);
                }
                newRef.triangles.insert(triangleI);
            }
        }
        for (const auto& pairI : pairsToTransfer) {
            auto& pair = pairs[pairI];
            if (pairToCollapseI == pairI)
                continue;
            for (const auto& oldPositionI : pairToCollapse.positions)
                std::replace(pair.positions.begin(), pair.positions.end(), oldPositionI, newPositionI);
            newRef.pairs.insert(pairI);
        }
        verticeToTransfer.clear();
        trianglesToTransfer.clear();
        pairsToTransfer.clear();
        newRef.quadricMatrix = {};
        for (const auto& triangleI : newRef.triangles) {
            const auto& triangle = triangles[triangleI];
            assert(!triangle.collapsed);
            newRef.quadricMatrix += triangle.quadricMatrix;
        }
        newRef.pairs.erase(pairToCollapseI);
        for (const auto& pairI : newRef.pairs)
            _UpdatePair(pairs[pairI]);
        _SortPairs();
#ifndef NDEBUG
        _CheckValidity();
#endif
    }
    const auto inputCount = triangles.size();
    std::erase_if(triangles, [this](const auto& a_Triangle) { return a_Triangle.collapsed; });
    const auto outputCount = triangles.size();
    consoleStream << "Input triangle count  : " << inputCount << '\n';
    consoleStream << "Output triangle count : " << outputCount << '\n';
    consoleStream << "Compression ratio     : " << outputCount / float(inputCount) << '\n';
    return _ReconstructPrimitive();
}

template <typename Accessor>
inline void PrimitiveOptimizer::_FromIndexed(const Accessor& a_Indice)
{
    for (uint32_t i = 0; i < a_Indice.GetSize(); i += 3)
        _PushTriangle({ a_Indice.at(i + 0), a_Indice.at(i + 1), a_Indice.at(i + 2) });
}

void PrimitiveOptimizer::_PushTriangle(const std::array<uint32_t, 3>& a_Indice)
{
    Triangle triangle = {};
    for (uint32_t i = 0; i < 3; i++) {
        VertexData vertexData = {};
        vertexData.position   = ConvertData<posType::length(), posType::value_type>(primitive->GetPositions(), a_Indice[i]);
        vertexData.normal     = hasNormals ? ConvertData<norType::length(), norType::value_type, true>(primitive->GetNormals(), a_Indice[i]) : norType {};
        vertexData.tangent    = hasTangents ? ConvertData<tanType::length(), tanType::value_type>(primitive->GetTangent(), a_Indice[i]) : tanType {};
        vertexData.texCoord0  = hasTexCoord0 ? ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord0(), a_Indice[i]) : texType {};
        vertexData.texCoord1  = hasTexCoord1 ? ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord1(), a_Indice[i]) : texType {};
        vertexData.texCoord2  = hasTexCoord2 ? ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord2(), a_Indice[i]) : texType {};
        vertexData.texCoord3  = hasTexCoord3 ? ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord3(), a_Indice[i]) : texType {};
        vertexData.color      = hasColors ? ConvertData<colType::length(), colType::value_type>(primitive->GetColors(), a_Indice[i]) : colType {};
        vertexData.joints     = hasJoints ? ConvertData<joiType::length(), joiType::value_type>(primitive->GetJoints(), a_Indice[i]) : joiType {};
        vertexData.weights    = hasWeights ? ConvertData<weiType::length(), weiType::value_type>(primitive->GetWeights(), a_Indice[i]) : weiType {};
        auto vertexI          = _InsertVertexData(vertexData);
        const auto& vertex    = vertice[vertexI];
        triangle.vertice[i]   = vertexI;
        references[vertex.position].vertice.insert(InsertUnique(vertice, vertex));
    }
    _UpdateTriangle(triangle);
    // Don't keep already collapsed triangles
    if (triangle.collapsed)
        return;
    triangle.originalNormal = triangle.plane.GetNormal();
    auto triangleI          = InsertUnique(triangles, triangle);
    for (const auto& vertexI : triangle.vertice) {
        const auto& vertex = vertice[vertexI];
        references[vertex.position].triangles.insert(triangleI);
    }
}

bool PrimitiveOptimizer::_TriangleIsCollapsed(const Triangle& a_Triangle) const
{
    const auto& posI0 = vertice[a_Triangle.vertice[0]].position;
    const auto& posI1 = vertice[a_Triangle.vertice[1]].position;
    const auto& posI2 = vertice[a_Triangle.vertice[2]].position;
    return posI0 == posI1
        || posI0 == posI2
        || posI1 == posI2;
}

uint64_t PrimitiveOptimizer::_InsertPosition(const glm::vec3& a_Position)
{
    auto positionI = InsertUnique(positions, a_Position, AlmostEqual {});
    references.resize(positions.size());
    return positionI;
}

uint64_t PrimitiveOptimizer::_InsertPair(const Pair& a_Pair)
{
    auto it = std::find_if(pairs.begin(), pairs.end(), [a_Pair](const auto& a_Val) { return a_Val == a_Pair; });
    if (it == pairs.end()) {
        pairs.push_back(a_Pair);
        auto pairI = pairs.size() - 1;
        references[a_Pair.positions[0]].pairs.insert(pairI);
        references[a_Pair.positions[1]].pairs.insert(pairI);
        _UpdatePair(pairs[pairI]); // Compute initial contraction cost
        return pairI;
    }
    return std::distance(pairs.begin(), it);
}

void PrimitiveOptimizer::_DeleteTriangle(const uint64_t& a_TriangleI)
{
    auto& triangle = triangles[a_TriangleI];
    for (auto& vertexI : triangle.vertice) {
        const auto& positionI = vertice[vertexI].position;
        references[positionI].triangles.erase(a_TriangleI);
    }
    triangle = {};
}

void PrimitiveOptimizer::_UpdatePair(Pair& a_Pair)
{
    const auto& posI0 = a_Pair.positions[0];
    const auto& posI1 = a_Pair.positions[1];
    auto q            = references[posI0].quadricMatrix + references[posI1].quadricMatrix;
    auto p0           = glm::vec4(positions[posI0], 1.f);
    auto p1           = glm::vec4(positions[posI1], 1.f);
    auto p2           = (p0 + p1) * 0.5f;
    auto error0       = ComputeError(q, p0);
    auto error1       = ComputeError(q, p1);
    auto error2       = ComputeError(q, p2);
    a_Pair.error      = error2;
    a_Pair.target     = 2;
    if (error1 < a_Pair.error) {
        a_Pair.error  = error1;
        a_Pair.target = 1;
    }
    if (error0 < a_Pair.error) {
        a_Pair.error  = error0;
        a_Pair.target = 0;
    }
}

void PrimitiveOptimizer::_UpdateTriangle(Triangle& a_Triangle)
{
    const auto& v0           = vertice[a_Triangle.vertice[0]];
    const auto& v1           = vertice[a_Triangle.vertice[1]];
    const auto& v2           = vertice[a_Triangle.vertice[2]];
    const auto& p0           = positions[v0.position];
    const auto& p1           = positions[v1.position];
    const auto& p2           = positions[v2.position];
    a_Triangle.plane         = Component::Plane(p0, glm::normalize(glm::cross(p1 - p0, p2 - p0)));
    a_Triangle.quadricMatrix = ComputeQuadricMatrix(a_Triangle);
    a_Triangle.collapsed     = _TriangleIsCollapsed(a_Triangle);
}

bool PrimitiveOptimizer::_CheckInversion(const Triangle& a_Triangle, const uint64_t& a_OldVertex, const uint64_t& a_NewVertex)
{
    std::array<uint64_t, 3> newTriangle = a_Triangle.vertice;
    for (auto& vertexI : newTriangle) {
        if (vertexI == a_OldVertex)
            vertexI = a_NewVertex;
    }
    auto& p0       = positions[vertice[newTriangle[0]].position];
    auto& p1       = positions[vertice[newTriangle[1]].position];
    auto& p2       = positions[vertice[newTriangle[2]].position];
    auto oldNormal = a_Triangle.plane.GetNormal();
    auto newNormal = glm::normalize(glm::cross(p2 - p0, p1 - p0));
    return glm::dot(oldNormal, newNormal) < 0.f;
}

VertexData PrimitiveOptimizer::_GetVertexData(const Vertex& a_V)
{
    return {
        .position  = positions[a_V.position],
        .normal    = hasNormals ? normals[a_V.normal] : norType {},
        .tangent   = hasTangents ? tangents[a_V.tangent] : tanType {},
        .texCoord0 = hasTexCoord0 ? texCoords[a_V.texCoord0] : texType {},
        .texCoord1 = hasTexCoord1 ? texCoords[a_V.texCoord1] : texType {},
        .texCoord2 = hasTexCoord2 ? texCoords[a_V.texCoord2] : texType {},
        .texCoord3 = hasTexCoord3 ? texCoords[a_V.texCoord3] : texType {},
        .color     = hasColors ? colors[a_V.color] : colType {},
        .joints    = hasJoints ? joints[a_V.joints] : joiType {},
        .weights   = hasWeights ? weights[a_V.weights] : weiType {},
    };
}

VertexData PrimitiveOptimizer::_MergeVertice(const Vertex& a_V0, const Vertex& a_V1, const float& a_X)
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
        .normal    = hasNormals ? (int64_t)InsertUnique(normals, a_Vd.normal, AlmostEqual {}) : -1,
        .tangent   = hasTangents ? (int64_t)InsertUnique(tangents, a_Vd.tangent, AlmostEqual {}) : -1,
        .texCoord0 = hasTexCoord0 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord0, AlmostEqual {}) : -1,
        .texCoord1 = hasTexCoord1 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord1, AlmostEqual {}) : -1,
        .texCoord2 = hasTexCoord2 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord2, AlmostEqual {}) : -1,
        .texCoord3 = hasTexCoord3 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord3, AlmostEqual {}) : -1,
        .color     = hasColors ? (int64_t)InsertUnique(colors, a_Vd.color, AlmostEqual {}) : -1,
        .joints    = hasJoints ? (int64_t)InsertUnique(joints, a_Vd.joints, AlmostEqual {}) : -1,
        .weights   = hasWeights ? (int64_t)InsertUnique(weights, a_Vd.weights, AlmostEqual {}) : -1
    };
    auto vertexI = InsertUnique(vertice, v);
    return *references[v.position].vertice.insert(vertexI).first;
}

void PrimitiveOptimizer::_SortPairs()
{
    std::sort(pairIndice.begin(), pairIndice.end(), [&pairs = pairs](auto& a_A, auto& a_B) {
        return pairs[a_A].error > pairs[a_B].error;
    });
}
}
