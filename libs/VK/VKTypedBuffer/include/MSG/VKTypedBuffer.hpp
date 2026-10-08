#pragma once

#include <MSG/VKBuffer.hpp>

#include <concepts>

namespace Msg {
class VKMemoryAllocator;
}

namespace Msg {
template <typename Type>
concept VKBufferType = std::is_trivially_copyable_v<Type>;

template <VKBufferType Type>
class VKTypedBuffer {
public:
    using value_type                      = Type;
    using size_type                       = size_t;
    using reference                       = Type&;
    using const_reference                 = const Type&;
    using pointer                         = Type*;
    using const_pointer                   = const Type*;
    static constexpr size_type value_size = sizeof(Type);
    VKTypedBuffer(
        const vk::raii::Device& a_Device, const vk::BufferUsageFlags& a_Usage,
        const size_type& a_Count,
        VKMemoryAllocator& a_MemoryAllocator, const vk::MemoryPropertyFlags& a_MemoryProperties);
    std::span<Type> Map(const size_type& a_ElemOffset = 0, const size_type& a_ElemCount = VK_WHOLE_SIZE);
    void Unmap();
    VKBuffer buffer;
    const size_type count; // the number of elements inside this typed buffer
};
}

#include <MSG/VKTypedBuffer.inl>