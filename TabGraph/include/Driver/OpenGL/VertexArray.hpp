/*
* @Author: gpinchon
* @Date:   2021-03-25 11:09:05
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-03-25 11:14:50
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Buffer/Accessor.hpp>
#include <Buffer/View.hpp>
#include <Driver/OpenGL/Buffer.hpp>
#include <Driver/OpenGL/ObjectHandle.hpp>

#include <memory>
#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward Declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace OpenGL {
class VertexArray {
public:
    using Handle = OpenGL::ObjectHandle;
    VertexArray();
    ~VertexArray();
    auto GetHandle() const
    {
        return _handle;
    }
    VertexArray& Bind();
    template<typename T>
    VertexArray& BindAccessor(const TabGraph::Buffer::TypedAccessor<T>& accessor, int index);
    VertexArray& BindAccessor(const TabGraph::Buffer::Accessor& accessor, int index);
    void Done();
    static void BindNone();

private:
    Handle _handle { 0 };
};
template<typename T>
static inline auto GetTypeValuesNbr() {
    static_assert("type not managed");
    return 0;
}
template<>
static inline auto GetTypeValuesNbr<float>() {
    return 1;
}
template<>
static inline auto GetTypeValuesNbr<unsigned>() {
    return 1;
}
template<>
static inline auto GetTypeValuesNbr<glm::vec2>() {
    return 2;
}
template<>
static inline auto GetTypeValuesNbr<glm::vec3>() {
    return 3;
}
template<>
static inline auto GetTypeValuesNbr<glm::vec4>() {
    return 4;
}

template<typename T>
static inline auto GetOpenGLType() {
    static_assert("type not managed");
    return GL_NONE;
}
template<>
static inline auto GetOpenGLType<float>() {
    return GL_FLOAT;
}
template<>
static inline auto GetOpenGLType<unsigned>() {
    return GL_UNSIGNED_INT;
}
template<>
static inline auto GetOpenGLType<glm::vec2>() {
    return GL_FLOAT;
}
template<>
static inline auto GetOpenGLType<glm::vec3>() {
    return GL_FLOAT;
}
template<>
static inline auto GetOpenGLType<glm::vec4>() {
    return GL_FLOAT;
}

template<typename T>
inline VertexArray& VertexArray::BindAccessor(const TabGraph::Buffer::TypedAccessor<T>& accessor, int index)
{
    auto bufferView(accessor.GetBufferView());
    if (bufferView == nullptr) {
        glDisableVertexAttribArray(index);
        return *this;
    }
    auto byteOffset(accessor.GetByteOffset());
    bufferView->Load();
    glEnableVertexAttribArray(index);
    glVertexAttribFormat(
        index,
        GetTypeValuesNbr<T>(),
        GetOpenGLType<T>(),
        accessor.GetNormalized(),
        0);
    glBindVertexBuffer(
        index,
        OpenGL::GetHandle(bufferView),
        accessor.GetByteOffset(),
        bufferView->GetByteStride() ? bufferView->GetByteStride() : sizeof(T));
    return *this;
}

inline static auto GetOpenGLType(const TabGraph::Buffer::Accessor::ComponentType& componentType) {
    switch (componentType)
    {
    case TabGraph::Buffer::Accessor::ComponentType::Int8:
        return GL_BYTE;
    case TabGraph::Buffer::Accessor::ComponentType::Uint8:
        return GL_UNSIGNED_BYTE;
    case TabGraph::Buffer::Accessor::ComponentType::Int16:
        return GL_SHORT;
    case TabGraph::Buffer::Accessor::ComponentType::Uint16:
        return GL_UNSIGNED_SHORT;
    case TabGraph::Buffer::Accessor::ComponentType::Float16:
        return GL_HALF_FLOAT;
    case TabGraph::Buffer::Accessor::ComponentType::Int32:
        return GL_INT;
    case TabGraph::Buffer::Accessor::ComponentType::Uint32:
        return GL_UNSIGNED_INT;
    case TabGraph::Buffer::Accessor::ComponentType::Float32:
        return GL_FLOAT;
    default:
        throw std::runtime_error("Unknown Buffer::Accessor::ComponentType");
    }
}

inline VertexArray& OpenGL::VertexArray::BindAccessor(const TabGraph::Buffer::Accessor& accessor, int index)
{
    auto bufferView(accessor.GetBufferView());
    if (bufferView == nullptr) {
        glDisableVertexAttribArray(index);
        return *this;
    }
    auto byteOffset(accessor.GetByteOffset());
    bufferView->Load();
    glEnableVertexAttribArray(index);
    glVertexAttribFormat(
        index,
        accessor.GetComponentNbr(),
        GetOpenGLType(accessor.GetComponentType()),
        accessor.GetNormalized(),
        0);
    glBindVertexBuffer(
        index,
        OpenGL::GetHandle(bufferView),
        accessor.GetByteOffset(),
        bufferView->GetByteStride() ? bufferView->GetByteStride() : accessor.GetComponentTypeSize());
    return *this;
}
}
