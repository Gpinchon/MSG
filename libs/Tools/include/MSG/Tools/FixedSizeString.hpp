#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <array>
#include <string>
#include <cstring>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Tools {
template <size_t MaxSize>
class FixedSizeString {
public:
    static constexpr auto max_size = MaxSize;
    FixedSizeString()
    {
        _memory.fill(0);
    };
    FixedSizeString(const FixedSizeString& a_Other)
        : _memory(a_Other._memory)
        , _size(a_Other._size)
    {
    }
    FixedSizeString(const char* a_Value)
        : FixedSizeString()
    {
        _size = std::min(_memory.size() - 1, std::strlen(a_Value)); // make sure to keep a closing zero
        _memory.fill(0);
        std::memcpy(_memory.data(), a_Value, _size);
    }
    FixedSizeString(const std::string& a_Value)
        : FixedSizeString(a_Value.c_str())
    {
    }
    size_t size() const
    {
        return _size;
    }
    operator std::string() const
    {
        return _memory.data();
    }
    operator std::string_view()
    {
        return { _memory.data(), _size };
    }
    operator const std::string_view() const
    {
        return { _memory.data(), _size };
    }
    bool operator!=(const std::string& a_Right) const
    {
        return std::string_view(*this) != a_Right;
    }

private:
    std::array<char, max_size> _memory;
    size_t _size = 0;
};
}