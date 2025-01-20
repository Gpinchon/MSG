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
#include <SG/Core/Property.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG {
class Buffer;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG::SG {
/** A view into a buffer generally representing a subset of the buffer. */
class BufferView : public Inherit<Object, BufferView> {
public:
    PROPERTY(size_t, ByteLength, 0);
    PROPERTY(size_t, ByteStride, 0);
    PROPERTY(int, ByteOffset, 0);
    PROPERTY(std::shared_ptr<Buffer>, Buffer, nullptr);

public:
    BufferView();
    BufferView(const BufferView&);
    /**
     * @brief Creates a new BufferView with a Buffer with size a_ByteOffset + a_ByteLength
     */
    BufferView(const int& a_ByteOffset, const size_t& a_ByteLength, const size_t& a_ByteStride = 0);
    BufferView(
        const std::shared_ptr<Buffer>& a_Buffer,
        const int& a_ByteOffset,
        const size_t& a_ByteLength,
        const size_t& a_ByteStride = 0)
        : Inherit()
    {
        SetBuffer(a_Buffer);
        SetByteOffset(a_ByteOffset);
        SetByteLength(a_ByteLength);
        SetByteStride(a_ByteStride);
    }
    ~BufferView() override = default;
    bool empty() const;
};
}
