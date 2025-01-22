#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <iterator>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
template <typename T>
class BufferTypedIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;
    BufferTypedIterator(std::byte* a_Ptr, size_t a_ByteStride = 0)
        : _ptr(a_Ptr)
        , _byteStride(a_ByteStride ? a_ByteStride : sizeof(T))
    {
    }
    reference operator*() { return *reinterpret_cast<T*>(_ptr); }
    reference operator*() const { return *reinterpret_cast<T*>(_ptr); }
    pointer operator->() { return reinterpret_cast<T*>(_ptr); }
    const pointer operator->() const { return reinterpret_cast<T*>(_ptr); }
    BufferTypedIterator operator+(size_t i)
    {
        auto tmp = *this;
        tmp._ptr += _byteStride * i;
        return tmp;
    }
    BufferTypedIterator operator-(size_t i)
    {
        auto tmp = *this;
        tmp._ptr -= _byteStride * i;
        return tmp;
    }
    BufferTypedIterator& operator++()
    {
        _ptr += _byteStride;
        return *this;
    }
    BufferTypedIterator operator++(int)
    {
        auto tmp = *this;
        ++(tmp);
        return tmp;
    }

    BufferTypedIterator& operator--()
    {
        _ptr -= _byteStride;
        return *this;
    }
    BufferTypedIterator operator--(int)
    {
        auto tmp = *this;
        --(tmp);
        return tmp;
    }
    bool operator==(const BufferTypedIterator& a_Right) { return _ptr == a_Right._ptr; }
    bool operator!=(const BufferTypedIterator& a_Right) { return !(*this == a_Right); }
    friend BufferTypedIterator operator+(const BufferTypedIterator& a_Left, size_t a_Right) { return BufferTypedIterator(a_Left._ptr + (a_Left._byteStride * a_Right), a_Left._byteStride); }
    friend BufferTypedIterator operator-(const BufferTypedIterator& a_Left, size_t a_Right) { return BufferTypedIterator(a_Left._ptr - (a_Left._byteStride * a_Right), a_Left._byteStride); }

private:
    std::byte* _ptr { nullptr };
    const size_t _byteStride;
};
using BufferIterator = BufferTypedIterator<std::byte>;
}
