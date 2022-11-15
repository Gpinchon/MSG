/*
* @Author: gpinchon
* @Date:   2019-08-10 11:50:30
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:13
*/

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
