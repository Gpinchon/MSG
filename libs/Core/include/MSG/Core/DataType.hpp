#pragma once

#include <cassert>
#include <cstdint>
#include <limits>
#include <stdexcept>

namespace Msg::Core {
enum class DataWidth : uint8_t {
    Octets1 = 0b0001,
    Octets2 = 0b0010,
    Octets3 = 0b0011,
    Octets4 = 0b0100,
    Octets8 = 0b1000,
};

enum class DataSigned : uint8_t {
    Unsigned = 0b00,
    Signed   = 0b01,
    Floating = 0b11,
    DXT5     = 0b111
};

/**
 * @brief the type of data used for pixels and vertex data
 */
enum class DataType : uint8_t {
    None    = 0,
    Uint8   = (uint8_t(DataWidth::Octets1) << 4) | uint8_t(DataSigned::Unsigned),
    Int8    = (uint8_t(DataWidth::Octets1) << 4) | uint8_t(DataSigned::Signed),
    Uint16  = (uint8_t(DataWidth::Octets2) << 4) | uint8_t(DataSigned::Unsigned),
    Int16   = (uint8_t(DataWidth::Octets2) << 4) | uint8_t(DataSigned::Signed),
    Uint24  = (uint8_t(DataWidth::Octets3) << 4) | uint8_t(DataSigned::Unsigned),
    Int24   = (uint8_t(DataWidth::Octets3) << 4) | uint8_t(DataSigned::Signed),
    Uint32  = (uint8_t(DataWidth::Octets4) << 4) | uint8_t(DataSigned::Unsigned),
    Int32   = (uint8_t(DataWidth::Octets4) << 4) | uint8_t(DataSigned::Signed),
    Float16 = (uint8_t(DataWidth::Octets2) << 4) | uint8_t(DataSigned::Floating),
    Float32 = (uint8_t(DataWidth::Octets4) << 4) | uint8_t(DataSigned::Floating),
    DXT5    = (uint8_t(DataWidth::Octets1) << 4) | uint8_t(DataSigned::DXT5) // Special flag
};

inline uint8_t DataTypeSize(const DataType& a_Type)
{
    return uint16_t(a_Type) >> 4;
}
}
