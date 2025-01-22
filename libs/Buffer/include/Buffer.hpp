#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Core/Inherit.hpp>
#include <Core/Object.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
using BufferBase = std::vector<std::byte>;
/**
 * @brief this is just a buffer of raw bytes
 */
class Buffer : public BufferBase, public Core::Inherit<Core::Object, Buffer> {
public:
    Buffer();
    inline Buffer(const std::string& a_Name)
        : Buffer()
    {
        SetName(a_Name);
    }
    inline Buffer(const size_t& a_Size)
        : Buffer()
    {
        resize(a_Size);
    }
    inline Buffer(const std::vector<std::byte>& a_RawData)
        : Buffer()
    {
        BufferBase::operator=(a_RawData);
    }
    inline Buffer(const std::byte* a_RawDataPtr, const size_t a_DataByteSize)
        : Buffer()
    {
        BufferBase::operator=({ a_RawDataPtr, a_RawDataPtr + a_DataByteSize });
    }
    template <typename T>
    inline void push_back(const T& a_Value)
    {
        const auto offset = size();
        resize(offset + sizeof(T));
        memcpy(data() + offset, &a_Value, sizeof(T));
    }
};
}
