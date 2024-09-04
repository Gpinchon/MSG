/*
 * @Author: gpinchon
 * @Date:   2020-06-18 13:31:08
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-01 22:30:43
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>

#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
using BufferBase = std::vector<std::byte>;
/**
 * @brief this is just a buffer of raw bytes
 */
class Buffer : public BufferBase, public Inherit<Object, Buffer> {
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
