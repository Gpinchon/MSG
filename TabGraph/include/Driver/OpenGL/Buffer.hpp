/*
* @Author: gpinchon
* @Date:   2021-02-28 11:32:40
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-01 20:16:14
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Buffer/View.hpp>
#include <Driver/OpenGL/ObjectHandle.hpp>

#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// Class Declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Buffer {
class View::ImplGPU {
    READONLYPROPERTY(MappingMode, MappingMode, MappingMode::None);
    READONLYPROPERTY(size_t, MappingStart, 0);
    READONLYPROPERTY(size_t, MappingEnd, 0);
    READONLYPROPERTY(bool, Loaded, false);

public:
    ImplGPU() = default;
    ImplGPU(const ImplGPU&) = delete;
    auto begin()
    {
        return _mappingPointer;
    }
    auto end()
    {
        return _mappingPointer + _MappingEnd;
    }
    auto& at(const Buffer::View& buffer, size_t index)
    {
        assert(GetMappingStart() <= index && GetMappingEnd() >= index);
        return *(begin() + (index - GetMappingStart()));
    }
    using Handle = OpenGL::ObjectHandle;
    const Handle GetHandle() const;
    std::byte* GetMappingPtr();
    std::byte* MapRange(const Buffer::View& buffer, MappingMode mappingMode, size_t start, size_t end, bool invalidate = false);
    void Unmap(const Buffer::View& buffer);
    void FlushRange(const Buffer::View& buffer, size_t start, size_t end);
    void Load(const Buffer::View& buffer, std::byte* data = nullptr);
    void Unload();
    void Bind(Type type);
    void Done(Type type);
    static void BindNone(Type type);

private:
    void _SetHandle(uint32_t value);
    size_t _flushStart { 0 };
    size_t _flushEnd { 0 };
    std::byte* _mappingPointer { nullptr };
    Handle _handle { 0 };
};

}

namespace OpenGL {
TabGraph::Buffer::View::ImplGPU::Handle GetHandle(std::shared_ptr<TabGraph::Buffer::View> buffer);
void Bind(std::shared_ptr<TabGraph::Buffer::View>, TabGraph::Buffer::View::Type);
inline void Bind(std::shared_ptr<TabGraph::Buffer::View> buffer) {
    return Bind(buffer, buffer->GetType());
}
};
