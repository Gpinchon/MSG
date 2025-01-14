#pragma once

#include <cassert>
#include <stdexcept>

namespace TabGraph::SG {
enum class DataWidth : uint8_t {
    Octets1  = 0b00000001,
    Octets2  = 0b00000010,
    Octets3  = 0b00000011,
    Octets4  = 0b00000100,
    Octets8  = 0b00001000,
    MaxValue = uint8_t(Octets1) | uint8_t(Octets2) | uint8_t(Octets3) | uint8_t(Octets4) | uint8_t(Octets8)
};

enum class DataSigned : uint8_t {
    Unsigned = 0b00000000,
    Signed   = 0b00000001,
    Floating = 0b00000011,
    MaxValue = uint8_t(Unsigned) | uint8_t(Signed) | uint8_t(Floating)
};

/**
 * @brief the type of data used for pixels and vertex data
 */
enum class DataType : uint16_t {
    Uint8     = (uint16_t(DataWidth::Octets1) << 8) | uint16_t(DataSigned::Unsigned),
    Int8      = (uint16_t(DataWidth::Octets1) << 8) | uint16_t(DataSigned::Signed),
    Uint16    = (uint16_t(DataWidth::Octets2) << 8) | uint16_t(DataSigned::Unsigned),
    Int16     = (uint16_t(DataWidth::Octets2) << 8) | uint16_t(DataSigned::Signed),
    Uint24    = (uint16_t(DataWidth::Octets3) << 8) | uint16_t(DataSigned::Unsigned),
    Int24     = (uint16_t(DataWidth::Octets3) << 8) | uint16_t(DataSigned::Signed),
    Uint32    = (uint16_t(DataWidth::Octets4) << 8) | uint16_t(DataSigned::Unsigned),
    Int32     = (uint16_t(DataWidth::Octets4) << 8) | uint16_t(DataSigned::Signed),
    Float16   = (uint16_t(DataWidth::Octets2) << 8) | uint16_t(DataSigned::Floating),
    Float32   = (uint16_t(DataWidth::Octets4) << 8) | uint16_t(DataSigned::Floating),
    DXT5Block = 0b11111111, // Special flag
    MaxValue  = (uint16_t(DataWidth::MaxValue) << 8) | uint16_t(DataSigned::MaxValue),
    Unknown   = MaxValue + 1
};

inline auto DataTypeSize(const DataType& a_Type)
{
    assert(a_Type <= DataType::MaxValue);
    return uint16_t(a_Type) >> 8;
}
}
