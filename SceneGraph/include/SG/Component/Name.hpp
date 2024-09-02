#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#define __STDC_WANT_LIB_EXT1__ 1
#include <string.h>
#include <string>
#include <cstring>
#include <array>

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
    {
    }
    Name(const char* a_Value)
        : Name()
    {
        strncpy_s(_memory.data(), _memory.size(), a_Value, max_size - 1);
    }
    Name(const std::string& a_Value)
        : Name(a_Value.c_str())
    {
    }
    operator std::string() const
    {
        return _memory.data();
    }
    operator std::string_view()
    {
        return { _memory.data(), _memory.size() };
    }

private:
    std::array<char, max_size> _memory;
};
}
