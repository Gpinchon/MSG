#pragma once

#include <FBX/FBXNode.hpp>

#include <filesystem>
#include <string>

namespace FBX {
struct Header;

struct Document : public Node {
    virtual ~Document() = default;
    static Document* Parse(const std::filesystem::path path);
    Header* header;
    std::filesystem::path path;
    void Print() const override;

private:
    Document();
};
} // namespace FBX
