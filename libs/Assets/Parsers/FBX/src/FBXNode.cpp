#include <FBX/FBXNode.hpp>
#include <FBX/FBXObject.hpp>
#include <FBX/FBXProperty.hpp>
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
    for (const auto& subNodes : nodes) {
        for (const auto& subNode : subNodes.second)
            subNode->Print();
    }
    std::cout << "}," << std::endl;
}

Property& Node::Property(const size_t index)
{
    return *properties.at(index);
}
