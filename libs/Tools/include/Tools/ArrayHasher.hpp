#pragma once

#include <string_view>

namespace MSG::Tools {
template <typename ArrayType>
size_t HashArray(const ArrayType& a_Array)
{
    auto data = reinterpret_cast<const char*>(a_Array.data());
    auto size = a_Array.size() * sizeof(a_Array[0]);
    return std::hash<std::string_view> {}(std::string_view(data, size));
}
}
