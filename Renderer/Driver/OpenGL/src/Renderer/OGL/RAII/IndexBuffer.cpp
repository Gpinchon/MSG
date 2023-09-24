#include <Renderer/OGL/RAII/IndexBuffer.hpp>

#include <SG/Core/Buffer/Accessor.hpp>
#include <SG/Core/Primitive.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdexcept>

namespace TabGraph::Renderer::RAII {
template <unsigned L, typename T, bool Normalized = false>
static inline glm::vec<L, T> ConvertData(const SG::BufferAccessor& a_Accessor, size_t a_Index)
{
    const auto componentNbr = a_Accessor.GetComponentNbr();
    glm::vec<L, T> ret {};
    for (auto i = 0u; i < L && i < componentNbr; ++i) {
        switch (a_Accessor.GetComponentType()) {
        case SG::BufferAccessor::ComponentType::Int8:
            ret[i] = a_Accessor.GetComponent<glm::int8>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint8:
            ret[i] = a_Accessor.GetComponent<glm::uint8>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Int16:
            ret[i] = a_Accessor.GetComponent<glm::int16>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint16:
            ret[i] = a_Accessor.GetComponent<glm::uint16>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Uint32:
            ret[i] = a_Accessor.GetComponent<glm::uint32>(a_Index, i);
            break;
        case SG::BufferAccessor::ComponentType::Float16:
            ret[i] = glm::detail::toFloat32(a_Accessor.GetComponent<glm::detail::hdata>(a_Index, i));
            break;
        case SG::BufferAccessor::ComponentType::Float32:
            ret[i] = a_Accessor.GetComponent<glm::f32>(a_Index, i);
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

inline std::vector<unsigned> ConvertIndice(const SG::Primitive& a_Primitive)
{
    if (a_Primitive.GetIndices().empty())
        return {};
    std::vector<unsigned> indice(a_Primitive.GetIndices().GetSize());
    auto hasIndice = !a_Primitive.GetIndices().empty();
#ifdef _DEBUG
    assert(hasIndice);
#endif
    for (auto i = 0u; i < a_Primitive.GetIndices().GetSize(); ++i) {
        indice.at(i) = ConvertData<1, glm::uint32>(a_Primitive.GetIndices(), i).x;
    }
    return indice;
}

IndexBuffer::IndexBuffer(const SG::Primitive& a_Primitive)
{
    auto indice = ConvertIndice(a_Primitive);
    if (indice.empty()) return;
    glCreateBuffers(1, &handle);
    glNamedBufferStorage(handle, indice.size() * sizeof(unsigned), indice.data(), 0);
    indexFormat = GL_UNSIGNED_INT;
    indexCount  = indice.size();
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &handle);
}
void IndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
}
void IndexBuffer::BindNone() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
}
