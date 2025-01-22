#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>

#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Buffer;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
/** A view into a buffer generally representing a subset of the buffer. */
class BufferView : public Core::Inherit<Core::Object, BufferView> {
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
