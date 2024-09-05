#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#define __STDC_WANT_LIB_EXT1__ 1
#include <array>
#include <cstring>
#include <string.h>
#include <string>

////////////////////////////////////////////////////////////////////////////////
// Forward Declaration
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class Declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG::Component {
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
        strncpy_s(_memory.data(), _memory.size(), a_Value, _TRUNCATE);
        _size = strnlen_s(_memory.data(), _memory.size());
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
    size_t _size;
};
}
