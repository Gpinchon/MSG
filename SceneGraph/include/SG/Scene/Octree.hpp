/*
 * @Author: gpinchon
 * @Date:   2020-06-18 13:31:08
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-07-01 22:30:41
 */
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <array>
#include <cstddef>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
template <typename Type>
struct OctreeLeaf {
    static constexpr auto IsNode = false;
    std::vector<Type> storage;
};

template <typename Type, size_t Depth, size_t MaxDepth>
struct OctreeNode : OctreeLeaf<Type> {
    static_assert(MaxDepth >= 1);
    static constexpr auto IsNode = Depth < MaxDepth;
    using LeafType               = OctreeLeaf<Type>;
    using NodeType               = OctreeNode<Type, Depth + 1, MaxDepth>;
    using ChildrenType           = std::conditional<IsNode, NodeType, LeafType>::type;
    std::array<ChildrenType, 8> children;
};

template <typename Type, size_t MaxDepth = 2>
struct Octree : OctreeNode<Type, 0, MaxDepth> { };
}
