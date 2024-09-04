#pragma once

#include <stdexcept>

namespace TabGraph::SG {
/**
 * @brief the type of data used for pixels and vertex data
 */
enum class DataType {
    Unknown = -1,
    Uint8,
    Int8,
    Uint16,
    Int16,
    Uint32,
    Int32,
    Float16,
    Float32,
    DXT5Block,
    MaxValue
};

inline auto DataTypeSize(const DataType& a_Type)
{
    switch (a_Type) {
    case DataType::Uint8:
    case DataType::Int8:
        return 1;
    case DataType::Uint16:
    case DataType::Int16:
    case DataType::Float16:
        return 2;
    case DataType::Uint32:
    case DataType::Int32:
    case DataType::Float32:
        return 4;
    case DataType::DXT5Block:
        return 16;
    default:
        throw std::runtime_error("Unknown DataType");
    }
}
}
