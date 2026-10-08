#pragma once

namespace Msg {
template <VKBufferType Type>
inline VKTypedBuffer<Type>::VKTypedBuffer(
    const vk::raii::Device& a_Device, const vk::BufferUsageFlags& a_Usage,
    const size_type& a_Count,
    VKMemoryAllocator& a_MemoryAllocator, const vk::MemoryPropertyFlags& a_MemoryProperties)
    : buffer(a_Device, a_Usage, 0, value_size * a_Count, a_MemoryAllocator, a_MemoryProperties)
    , count(a_Count)
{
}

template <VKBufferType Type>
inline std::span<Type> Msg::VKTypedBuffer<Type>::Map(const size_type& a_ElemOffset, const size_type& a_ElemCount)
{
    size_type elemCount = a_ElemCount != VK_WHOLE_SIZE ? a_ElemCount : count;
    pointer ptr         = buffer.Map(a_ElemOffset * value_size, elemCount * value_size);
    return std::span<Type>(ptr, elemCount);
}

template <VKBufferType Type>
inline void Msg::VKTypedBuffer<Type>::Unmap()
{
    buffer.Unmap();
}
}