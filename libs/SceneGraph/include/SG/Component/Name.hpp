#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_WARNINGS 1
#include <array>
#include <cstddef>
#include <cstring>
#include <string.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG::Component {
class Name {
public:
    static constexpr auto max_size = 256;
    Name()
    {
        _memory.fill(0);
    };
    Name(const Name& a_Other)
        : _memory(a_Other._memory)
        , _size(a_Other._size)
    {
    }
    Name(const char* a_Value)
        : Name()
    {
        _size = std::min(_memory.size() - 1, std::strlen(a_Value)); // make sure to keep a closing zero
        std::memcpy(_memory.data(), a_Value, _size);
    }
    Name(const std::string& a_Value)
        : Name(a_Value.c_str())
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
