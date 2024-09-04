/*
 * @Author: gpinchon
 * @Date:   2021-07-09 21:56:20
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-21 21:50:18
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
template <typename T>
class TypedBufferIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;
    TypedBufferIterator(std::byte* a_Ptr, size_t a_ByteStride = 0)
        : _ptr(a_Ptr)
        , _byteStride(a_ByteStride ? a_ByteStride : sizeof(T))
    {
    }
    reference operator*() { return *reinterpret_cast<T*>(_ptr); }
    const reference operator*() const { return *reinterpret_cast<T*>(_ptr); }
    pointer operator->() { return reinterpret_cast<T*>(_ptr); }
    const pointer operator->() const { return reinterpret_cast<T*>(_ptr); }
    TypedBufferIterator operator+(size_t i)
    {
        auto tmp = *this;
        tmp._ptr += _byteStride * i;
        return tmp;
    }
    TypedBufferIterator operator-(size_t i)
    {
        auto tmp = *this;
        tmp._ptr -= _byteStride * i;
        return tmp;
    }
    TypedBufferIterator& operator++()
    {
        _ptr += _byteStride;
        return *this;
    }
    TypedBufferIterator operator++(int)
    {
        auto tmp = *this;
        ++(tmp);
        return tmp;
    }

    TypedBufferIterator& operator--()
    {
        _ptr -= _byteStride;
        return *this;
    }
    TypedBufferIterator operator--(int)
    {
        auto tmp = *this;
        --(tmp);
        return tmp;
    }
    bool operator==(const TypedBufferIterator& a_Right) { return _ptr == a_Right._ptr; }
    bool operator!=(const TypedBufferIterator& a_Right) { return !(*this == a_Right); }
    friend TypedBufferIterator operator+(const TypedBufferIterator& a_Left, size_t a_Right) { return TypedBufferIterator(a_Left._ptr + (a_Left._byteStride * a_Right), a_Left._byteStride); }
    friend TypedBufferIterator operator-(const TypedBufferIterator& a_Left, size_t a_Right) { return TypedBufferIterator(a_Left._ptr - (a_Left._byteStride * a_Right), a_Left._byteStride); }

private:
    std::byte* _ptr { nullptr };
    const size_t _byteStride;
};
using BufferIterator = TypedBufferIterator<std::byte>;
}
