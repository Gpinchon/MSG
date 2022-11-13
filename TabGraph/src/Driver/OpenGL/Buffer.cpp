#include <Driver/OpenGL/Buffer.hpp>

#include <cassert>
#include <array>
#include <GL/glew.h>

using namespace TabGraph;

static inline auto GLMappingMode(Buffer::View::MappingMode mode) {
    static const std::array<GLbitfield, (int)Buffer::View::MappingMode::MaxValue> MappingModeLUT = {
        GL_MAP_READ_BIT,
        GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT,
        GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_FLUSH_EXPLICIT_BIT
    };
    return MappingModeLUT.at(int(mode));
}


static inline auto GLType(Buffer::View::Type type) {
    static const std::array<GLenum, (int)Buffer::View::Type::MaxValue> TypeLUT = {
    GL_ARRAY_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_TEXTURE_BUFFER
    };
    return TypeLUT.at(int(type));
}

static inline auto GLMode(Buffer::View::Mode mode) {
    static const std::array<GLbitfield, (int)Buffer::View::Mode::MaxValue> ModeLUT = {
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT | GL_CLIENT_STORAGE_BIT,
        GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT,
        0
    };
    return ModeLUT.at(int(mode));
}

const Buffer::View::ImplGPU::Handle Buffer::View::ImplGPU::GetHandle() const
{
    return _handle;
}

std::byte* Buffer::View::ImplGPU::GetMappingPtr()
{
    return _mappingPointer;
}

std::byte* Buffer::View::ImplGPU::MapRange(const Buffer::View& buffer, MappingMode mappingMode, size_t start, size_t end, bool invalidate)
{
    if (GetMappingMode() != MappingMode::None) {
        //this is a remap
        Unmap(buffer);
    }
    Bind(buffer.GetType());
    if (invalidate)
        _mappingPointer = (std::byte*)glMapBufferRange(GLType(buffer.GetType()), start, end, GLMappingMode(mappingMode) | GL_MAP_INVALIDATE_RANGE_BIT);
    else
        _mappingPointer = (std::byte*)glMapBufferRange(GLType(buffer.GetType()), start, end, GLMappingMode(mappingMode));
    _SetMappingMode(mappingMode);
    _SetMappingStart(start);
    _SetMappingEnd(end);
    Done(buffer.GetType());
    return _mappingPointer;
}

void Buffer::View::ImplGPU::Unmap(const Buffer::View& buffer)
{
    Bind(buffer.GetType());
    glUnmapBuffer(GLType(buffer.GetType()));
    _SetMappingMode(MappingMode::None);
    _SetMappingStart(0);
    _SetMappingEnd(0);
    Done(buffer.GetType());
}

void Buffer::View::ImplGPU::FlushRange(const Buffer::View& buffer, size_t start, size_t end)
{
    Bind(buffer.GetType());
    glFlushMappedBufferRange(GLType(buffer.GetType()), start, end);
    Done(buffer.GetType());
}

void Buffer::View::ImplGPU::Load(const Buffer::View& buffer, std::byte* data)
{
    if (GetLoaded()) return;
    GLuint id;
    glCreateBuffers(1, &id);
    _SetHandle(id);
    Bind(buffer.GetType());
    glBufferStorage(GLType(buffer.GetType()), buffer.GetByteLength(), data, GLMode(buffer.GetMode()));
    Done(buffer.GetType());
    if (buffer.GetMode() == Mode::Persistent)
        MapRange(buffer, buffer.GetPersistentMappingMode(), 0, buffer.GetByteLength());
    _SetLoaded(true);
}

void Buffer::View::ImplGPU::Unload()
{
    uint32_t handle = GetHandle();
    glDeleteBuffers(1, &handle);
    _SetHandle(0);
    _SetLoaded(false);
}

void Buffer::View::ImplGPU::Bind(Type type)
{
    glBindBuffer(GLType(type), GetHandle());
}

void Buffer::View::ImplGPU::Done(Type type)
{
    BindNone(type);
}

void Buffer::View::ImplGPU::BindNone(Type type)
{
    glBindBuffer(GLType(type), 0);
}

void Buffer::View::ImplGPU::_SetHandle(uint32_t value)
{
    _handle = value;
}

Buffer::View::ImplGPU::Handle OpenGL::GetHandle(std::shared_ptr<Buffer::View> buffer)
{
    return buffer->GetImplGPU()->GetHandle();
}

void OpenGL::Bind(std::shared_ptr<Buffer::View> buffer, Buffer::View::Type type)
{
    if (buffer == nullptr)
        Buffer::View::ImplGPU::BindNone(type);
    else
        buffer->GetImplGPU()->Bind(buffer->GetType());
}
