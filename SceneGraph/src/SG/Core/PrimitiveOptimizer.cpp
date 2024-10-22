#include <SG/Core/Primitive.hpp>
#include <SG/Core/PrimitiveOptimizer.hpp>
#include <Tools/Debug.hpp>

#include <algorithm>
#include <unordered_set>

namespace TabGraph::SG {
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

template <typename T>
size_t InsertUnique(std::vector<T>& a_Vec, const T& a_Data)
{
    auto it = std::find(a_Vec.begin(), a_Vec.end(), a_Data);
    if (it == a_Vec.end()) {
        a_Vec.push_back(a_Data);
        return a_Vec.size() - 1;
    }
    return std::distance(a_Vec.begin(), it);
}

void TabGraph::SG::PrimitiveOptimizer::_PushTriangle(const uint32_t& a_Index)
{
    Triangle triangle = {};
    for (uint32_t i = 0; i < 3; i++) {
        auto vi = a_Index + i;
        Vertex vertex;
        vertex.position  = InsertUnique(positions, ConvertData<posType::length(), posType::value_type>(primitive->GetPositions(), vi));
        vertex.normal    = hasNormals ? InsertUnique(normals, ConvertData<norType::length(), norType::value_type, true>(primitive->GetNormals(), vi)) : -1;
        vertex.tangent   = hasTangents ? InsertUnique(tangents, ConvertData<tanType::length(), tanType::value_type>(primitive->GetTangent(), vi)) : -1;
        vertex.texCoord0 = hasTexCoord0 ? InsertUnique(texCoords, ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord0(), vi)) : -1;
        vertex.texCoord1 = hasTexCoord1 ? InsertUnique(texCoords, ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord1(), vi)) : -1;
        vertex.texCoord2 = hasTexCoord2 ? InsertUnique(texCoords, ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord2(), vi)) : -1;
        vertex.texCoord3 = hasTexCoord3 ? InsertUnique(texCoords, ConvertData<texType::length(), texType::value_type>(primitive->GetTexCoord3(), vi)) : -1;
        vertex.color     = hasColors ? InsertUnique(colors, ConvertData<colType::length(), colType::value_type>(primitive->GetColors(), vi)) : -1;
        vertex.joints    = hasJoints ? InsertUnique(joints, ConvertData<joiType::length(), joiType::value_type>(primitive->GetJoints(), vi)) : -1;
        vertex.weights   = hasWeights ? InsertUnique(weights, ConvertData<weiType::length(), weiType::value_type>(primitive->GetWeights(), vi)) : -1;
        triangle.at(i)   = InsertUnique(vertice, vertex);
    }
    _UpdateTriangle(triangle);
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

PrimitiveOptimizer::PrimitiveOptimizer(const std::shared_ptr<Primitive>& a_Primitive)
    : primitive(a_Primitive)
{
    if (primitive->GetDrawingMode() != SG::Primitive::DrawingMode::Triangles) {
        errorLog("Mesh optimization only available for triangulated meshes");
        return;
    }
    positions.reserve(primitive->GetPositions().GetSize());
    normals.reserve(primitive->GetNormals().GetSize());
    tangents.reserve(primitive->GetTangent().GetSize());
    texCoords.reserve(primitive->GetTexCoord0().GetSize());
    colors.reserve(primitive->GetColors().GetSize());
    joints.reserve(primitive->GetJoints().GetSize());
    weights.reserve(primitive->GetWeights().GetSize());

    if (!primitive->GetIndices().empty()) {
        if (primitive->GetIndices().GetComponentType() == SG::DataType::Uint32)
            _FromIndexed(TypedBufferAccessor<uint32_t>(primitive->GetIndices()));
        else if (primitive->GetIndices().GetComponentType() == SG::DataType::Uint16)
            _FromIndexed(TypedBufferAccessor<uint16_t>(primitive->GetIndices()));
    } else {
        for (uint32_t i = 0; i < primitive->GetPositions().GetSize(); i += 3)
            _PushTriangle(i);
    }

    float threshold = 0.1;
    for (auto& triangle : triangles) {
        // Add edges to valid pairs
        InsertUnique(pairs, { triangle[0], triangle[1] });
        InsertUnique(pairs, { triangle[1], triangle[2] });
        InsertUnique(pairs, { triangle[2], triangle[0] });
        // Compute Q matrices
        for (auto& vi : triangle) {
            auto& vertex = vertice[vi];
            vertex.quadricMatrix += triangle.quadricMatrix;
        }
    }
    // Add close vertice to valid pairs
    // for (uint64_t vIndex = 0; vIndex < vertice.size(); ++vIndex) {
    //     auto& v0 = vertice[vIndex];
    //     auto& p0 = positions[v0.position];
    //     for (uint64_t vIndex1 = 0; vIndex1 < vertice.size(); ++vIndex1) {
    //         auto& v1 = vertice[vIndex1];
    //         auto& p1 = positions[v1.position];
    //         if (glm::distance(p0, p1) < threshold)
    //             InsertUnique(pairs, { vIndex, vIndex1 });
    //     }
    // }

    // Compute initial contraction cost
    for (auto& pair : pairs)
        _UpdatePair(pair);
}

std::shared_ptr<Primitive> PrimitiveOptimizer::operator()(const float& a_Aggressivity)
{
    auto target = pairs.size() * std::max(1 - a_Aggressivity, 0.1f);
    while (pairs.size() > target) {
        Pair pair         = pairs.back();
        auto newVertIndex = _InsertVertexData(pair.target);
        auto& newVertex   = vertice[newVertIndex];
        for (auto& vi : pair) {
            auto& vertex = vertice[vi];
            for (auto& ti : vertex.triangles) {
                auto& triangle = triangles[ti];
                for (auto& v : triangle) {
                    if (v == vi)
                        v = newVertIndex;
                }
                _UpdateTriangle(triangle);
            }
            newVertex.quadricMatrix = {};
            for (auto& ti : newVertex.triangles) {
                auto& triangle = triangles[ti];
                if (triangle.Valid())
                    newVertex.quadricMatrix += ComputeQuadricMatrix(triangle);
            }
        }
        pairs.pop_back();
        for (auto& p : pairs) {
            for (auto& v : p) {
                if (v == pair[0] || v == pair[1]) {
                    v = newVertIndex;
                    _UpdatePair(p);
                }
            }
        }
        std::sort(pairs.begin(), pairs.end(), [](const auto& a_Lhs, const auto& a_Rhs) { return a_Lhs.error > a_Rhs.error; });
    }

    // Generate new primitive
    auto buffer = std::make_shared<SG::Buffer>();
    buffer->reserve(triangles.size() * 3);
    for (auto& triangle : triangles) {
        if (triangle.Valid()) { // Check for collapsed triangles
            for (auto& v : triangle) {
                buffer->push_back(_GetVertexData(vertice[v]));
            }
        }
    }
    auto bufferView   = std::make_shared<SG::BufferView>(buffer, 0, buffer->size(), sizeof(VertexData));
    auto newPrimitive = std::make_shared<SG::Primitive>();
    auto vertexCount  = buffer->size() / sizeof(VertexData);
    newPrimitive->SetPositions({ bufferView, offsetof(VertexData, position), vertexCount, DataType::Float32, posType::length() });
    if (hasNormals)
        newPrimitive->SetNormals({ bufferView, offsetof(VertexData, normal), vertexCount, DataType::Float32, norType::length() });
    if (hasTangents)
        newPrimitive->SetTangent({ bufferView, offsetof(VertexData, tangent), vertexCount, DataType::Float32, tanType::length() });
    if (hasTexCoord0)
        newPrimitive->SetTexCoord0({ bufferView, offsetof(VertexData, texCoord0), vertexCount, DataType::Float32, texType::length() });
    if (hasTexCoord1)
        newPrimitive->SetTexCoord1({ bufferView, offsetof(VertexData, texCoord1), vertexCount, DataType::Float32, texType::length() });
    if (hasTexCoord2)
        newPrimitive->SetTexCoord2({ bufferView, offsetof(VertexData, texCoord2), vertexCount, DataType::Float32, texType::length() });
    if (hasTexCoord3)
        newPrimitive->SetTexCoord3({ bufferView, offsetof(VertexData, texCoord3), vertexCount, DataType::Float32, texType::length() });
    if (hasColors)
        newPrimitive->SetColors({ bufferView, offsetof(VertexData, color), vertexCount, DataType::Float32, colType::length() });
    if (hasJoints)
        newPrimitive->SetJoints({ bufferView, offsetof(VertexData, joints), vertexCount, DataType::Float32, joiType::length() });
    if (hasWeights)
        newPrimitive->SetWeights({ bufferView, offsetof(VertexData, weights), vertexCount, DataType::Float32, weiType::length() });
    newPrimitive->ComputeBoundingVolume();
    return newPrimitive;
}

template <typename Accessor>
inline void PrimitiveOptimizer::_FromIndexed(const Accessor& a_Indice)
{
    for (uint32_t i = 0; i < a_Indice.GetSize(); i += 3)
        _PushTriangle(a_Indice.at(i));
}

void PrimitiveOptimizer::_UpdatePair(Pair& a_Pair)
{
    auto& v0      = vertice[a_Pair[0]];
    auto& v1      = vertice[a_Pair[1]];
    auto q        = v0.quadricMatrix + v1.quadricMatrix;
    auto p0       = glm::vec4(positions[v0.position], 1.f);
    auto p1       = glm::vec4(positions[v1.position], 1.f);
    auto p2       = (p0 + p1) * 0.5f;
    auto error0   = ComputeError(q, p0);
    auto error1   = ComputeError(q, p1);
    auto error2   = ComputeError(q, p2);
    a_Pair.target = _MergeVertice(v0, v1);
    a_Pair.error  = error2;
    if (error1 < a_Pair.error) {
        a_Pair.target = _GetVertexData(v1);
        a_Pair.error  = error1;
    }
    if (error0 < a_Pair.error) {
        a_Pair.target = _GetVertexData(v0);
        a_Pair.error  = error0;
    }
}

void PrimitiveOptimizer::_UpdateTriangle(Triangle& a_Triangle)
{
    auto& p0                 = positions[vertice[a_Triangle[0]].position];
    auto& p1                 = positions[vertice[a_Triangle[1]].position];
    auto& p2                 = positions[vertice[a_Triangle[2]].position];
    a_Triangle.plane         = Component::Plane(p0, glm::normalize(glm::cross(p2 - p0, p1 - p0)));
    a_Triangle.center        = (p0 + p1 + p2) / 3.f;
    a_Triangle.quadricMatrix = ComputeQuadricMatrix(a_Triangle);
    auto tIndex              = InsertUnique(triangles, a_Triangle);
    for (auto& v : a_Triangle) {
        vertice[v].triangles.insert(tIndex);
    }
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

int64_t PrimitiveOptimizer::_InsertVertexData(const VertexData& a_Vd)
{
    Vertex v = {
        .position  = (int64_t)InsertUnique(positions, a_Vd.position),
        .normal    = hasNormals ? (int64_t)InsertUnique(normals, a_Vd.normal) : -1,
        .tangent   = hasTangents ? (int64_t)InsertUnique(tangents, a_Vd.tangent) : -1,
        .texCoord0 = hasTexCoord0 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord0) : -1,
        .texCoord1 = hasTexCoord1 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord1) : -1,
        .texCoord2 = hasTexCoord2 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord2) : -1,
        .texCoord3 = hasTexCoord3 ? (int64_t)InsertUnique(texCoords, a_Vd.texCoord3) : -1,
        .color     = hasColors ? (int64_t)InsertUnique(colors, a_Vd.color) : -1,
        .joints    = hasJoints ? (int64_t)InsertUnique(joints, a_Vd.joints) : -1,
        .weights   = hasWeights ? (int64_t)InsertUnique(weights, a_Vd.weights) : -1
    };
    return InsertUnique(vertice, v);
}
}
