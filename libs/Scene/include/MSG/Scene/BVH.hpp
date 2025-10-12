#pragma once

#include <MSG/BoundingVolume.hpp>

#include <optional>
#include <vector>

namespace Msg {
struct BVHNode {
    BoundingVolume bounds;
    uint32_t objectIndex = -1u;
    uint32_t parentIndex = -1u;
    uint32_t child1      = -1u;
    uint32_t child2      = -1u;
    bool isLeaf          = false;
};

template <typename T>
class BVH {
public:
    void Clear();
    uint32_t AllocateInternalNode();
    uint32_t AllocateLeafNode(const uint32_t& a_ObjectIndex, const BoundingVolume& a_BV);
    uint32_t PickBestSibling(const uint32_t& a_Node);
    template <typename... Args>
    void InsertLeaf(const BoundingVolume& a_BV, Args... a_Args);
    float ComputeCost() const;
    template <typename Op>
    void Visit(Op& a_Op) const;
    template <typename Op>
    void Visit(Op& a_Op);
    std::vector<BVHNode> nodes;
    std::vector<T> objects;
    uint32_t rootIndex = -1u;
};

template <typename T>
void BVH<T>::Clear()
{
    nodes.clear();
    objects.clear();
    rootIndex = -1u;
}
template <typename T>
uint32_t BVH<T>::AllocateInternalNode()
{
    nodes.emplace_back();
    return nodes.size() - 1u;
}
template <typename T>
uint32_t BVH<T>::AllocateLeafNode(const uint32_t& a_ObjectIndex, const BoundingVolume& a_BV)
{
    nodes.emplace_back(BVHNode {
        .bounds      = a_BV,
        .objectIndex = a_ObjectIndex,
        .isLeaf      = true,
    });
    return nodes.size() - 1u;
}
template <typename T>
uint32_t BVH<T>::PickBestSibling(const uint32_t& a_Node)
{
    auto& newNode        = nodes[a_Node];
    float newNodeArea    = newNode.bounds.Area();
    uint32_t bestSibling = -1u;
    float bestCost       = std::numeric_limits<float>::max();
    struct Candidate {
        uint32_t node;
        float inheritedCost;
    };
    std::vector<Candidate> stack;
    stack.emplace_back(Candidate { rootIndex, 0.f });
    while (!stack.empty()) {
        auto currentNode   = stack.back().node;
        auto inheritedCost = stack.back().inheritedCost;
        auto combinedBV    = nodes[currentNode].bounds + newNode.bounds;
        auto directCost    = combinedBV.Area();
        auto cost          = directCost + inheritedCost;
        stack.pop_back();
        if (cost < bestCost) {
            bestCost    = cost;
            bestSibling = currentNode;
        }
        inheritedCost += directCost - nodes[currentNode].bounds.Area();
        if ((newNodeArea + inheritedCost) < bestCost && !nodes[currentNode].isLeaf) {
            stack.emplace_back(Candidate { nodes[currentNode].child1, inheritedCost });
            stack.emplace_back(Candidate { nodes[currentNode].child2, inheritedCost });
        }
    }
    return bestSibling;
}
template <typename T>
template <typename... Args>
void BVH<T>::InsertLeaf(const BoundingVolume& a_BV, Args... a_Args)
{
    objects.emplace_back(std::forward<Args>(a_Args)...);
    uint32_t leafIndex = AllocateLeafNode(
        objects.size() - 1u,
        a_BV);
    if (leafIndex == 0) {
        rootIndex = leafIndex;
        return;
    }
    uint32_t sibling = PickBestSibling(leafIndex);
    // Create a new parent
    uint32_t oldParent           = nodes[sibling].parentIndex;
    uint32_t newParent           = AllocateInternalNode();
    nodes[newParent].parentIndex = oldParent;
    nodes[newParent].bounds      = a_BV + nodes[sibling].bounds;
    // Connect new leaf and sibling to new parent
    nodes[newParent].child1      = sibling;
    nodes[newParent].child2      = leafIndex;
    nodes[sibling].parentIndex   = newParent;
    nodes[leafIndex].parentIndex = newParent;
    if (oldParent != -1u) {
        // The sibling was not the root
        if (nodes[oldParent].child1 == sibling)
            nodes[oldParent].child1 = newParent;
        else
            nodes[oldParent].child2 = newParent;
    } else // The sibling was the root
        rootIndex = newParent;
    // Walk back up the tree refitting AABBs
    for (auto index = nodes[leafIndex].parentIndex; index != -1u; index = nodes[index].parentIndex) {
        int child1          = nodes[index].child1;
        int child2          = nodes[index].child2;
        nodes[index].bounds = nodes[child1].bounds + nodes[child2].bounds;
    }
}
template <typename T>
float BVH<T>::ComputeCost() const
{
    float cost = 0;
    for (auto& node : nodes) {
        if (!node.isLeaf) // Leaf area doesn't matter
            cost += node.bounds.Area();
    }
    return cost;
}
template <typename T>
template <typename Op>
void BVH<T>::Visit(Op& a_Op) const
{
    std::vector<uint32_t> stack;
    stack.emplace_back(rootIndex);
    while (!stack.empty()) {
        auto currentIndex = stack.back();
        auto& currentNode = nodes[currentIndex];
        stack.pop_back();
        if (a_Op(*this, currentNode) && !currentNode.isLeaf) {
            stack.emplace_back(currentNode.child1);
            stack.emplace_back(currentNode.child2);
        }
    }
}
template <typename T>
template <typename Op>
void BVH<T>::Visit(Op& a_Op)
{
    std::vector<uint32_t> stack;
    stack.emplace_back(rootIndex);
    while (!stack.empty()) {
        auto currentIndex = stack.back();
        auto& currentNode = nodes[currentIndex];
        stack.pop_back();
        if (a_Op(*this, currentNode) && !currentNode.isLeaf) {
            stack.emplace_back(currentNode.child1);
            stack.emplace_back(currentNode.child2);
        }
    }
}
}