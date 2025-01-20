#pragma once

#include <cassert>
#include <stdexcept>

namespace MSG::SG {
enum class DataWidth : uint8_t {
    Unknown = std::numeric_limits<uint8_t>::max(),
    Octets1 = 0b0001,
    Octets2 = 0b0010,
    Octets3 = 0b0011,
    Octets4 = 0b0100,
    Octets8 = 0b1000,
};

enum class DataSigned : uint8_t {
    Unknown  = std::numeric_limits<uint8_t>::max(),
    Unsigned = 0b00,
    Signed   = 0b01,
    Floating = 0b11,
};

/**
 * @brief the type of data used for pixels and vertex data
 */
enum class DataType : uint8_t {
    Unknown   = std::numeric_limits<uint8_t>::max(),
    None      = 0,
    Uint8     = (uint8_t(DataWidth::Octets1) << 4) | uint8_t(DataSigned::Unsigned),
    Int8      = (uint8_t(DataWidth::Octets1) << 4) | uint8_t(DataSigned::Signed),
    Uint16    = (uint8_t(DataWidth::Octets2) << 4) | uint8_t(DataSigned::Unsigned),
    Int16     = (uint8_t(DataWidth::Octets2) << 4) | uint8_t(DataSigned::Signed),
    Uint24    = (uint8_t(DataWidth::Octets3) << 4) | uint8_t(DataSigned::Unsigned),
    Int24     = (uint8_t(DataWidth::Octets3) << 4) | uint8_t(DataSigned::Signed),
    Uint32    = (uint8_t(DataWidth::Octets4) << 4) | uint8_t(DataSigned::Unsigned),
    Int32     = (uint8_t(DataWidth::Octets4) << 4) | uint8_t(DataSigned::Signed),
    Float16   = (uint8_t(DataWidth::Octets2) << 4) | uint8_t(DataSigned::Floating),
    Float32   = (uint8_t(DataWidth::Octets4) << 4) | uint8_t(DataSigned::Floating),
    DXT5Block = 0b11111111, // Special flag
};

inline uint8_t DataTypeSize(const DataType& a_Type)
{
    assert(a_Type != DataType::Unknown);
    return uint16_t(a_Type) >> 4;
}
}
