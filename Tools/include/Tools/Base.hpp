#pragma once
#include <string>
#include <vector>
#include <cstddef>

namespace TabGraph::Tools {
namespace Base64 {
    std::string Encode(const std::vector<std::byte>& data);
    std::vector<std::byte> Decode(const std::string& data);
}

namespace Base32 {
    std::string Encode(const std::vector<std::byte>& data);
    std::vector<std::byte> Decode(const std::string& data);
}
}
