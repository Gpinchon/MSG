/*
* @Author: gpinchon
* @Date:   2019-08-10 11:21:46
* @Last Modified by:   gpinchon
* @Last Modified time: 2019-08-11 15:13:49
*/

#include "Parser/FBX/FBXNode.hpp"
#include "Parser/FBX/FBXObject.hpp"
#include "Parser/FBX/FBXProperty.hpp"
#include <iostream>
#include <vector>

using namespace FBX;

std::shared_ptr<Node> Node::Create()
{
    auto ptr = std::shared_ptr<Node>(new Node());
    return ptr;
}

std::vector<std::shared_ptr<Node>>& Node::SubNodes(const std::string& name)
{
    return nodes[name];
}

std::shared_ptr<Node> Node::SubNode(const std::string& name)
{
    auto nodesVector = SubNodes(name);
    if (nodesVector.size() > 0)
        return nodesVector.at(0);
    return nullptr;
}

void Node::Print() const
{
    if (Name().empty())
        std::cout << "{\n";
    else
        std::cout << "\"" + Name() + "\": {\n";
    for (const auto& property : properties)
        property->Print();
    for (const auto subNodes : nodes) {
        for (const auto& subNode : subNodes.second)
            subNode->Print();
    }
    std::cout << "}," << std::endl;
}

Property& Node::Property(const size_t index)
{
    return *properties.at(index);
}