/*
* @Author: gpinchon
* @Date:   2019-08-10 11:20:21
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-01-11 08:45:12
*/

#pragma once

#include "Parser/FBX/FBXObject.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace FBX {

struct Node : public FBX::Object {
    virtual ~Node() = default;
    static std::shared_ptr<Node> Create();
    /** @return a vector containing all nodes with this name */
    std::vector<std::shared_ptr<Node>>& SubNodes(const std::string& name);
    /** @return the first node named with this name */
    std::shared_ptr<Node> SubNode(const std::string& name);
    /** @return the property at this index */
    class Property& Property(const size_t index);
    virtual void Print() const override;

    std::vector<std::shared_ptr<class Property>> properties;
    std::map<std::string, std::vector<std::shared_ptr<Node>>> nodes;

protected:
    Node() = default;
};
} // namespace FBX
