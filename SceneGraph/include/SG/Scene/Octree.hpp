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
#include <SG/Component/BoundingVolume.hpp>

#include <array>
#include <cstddef>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
static constexpr auto OctreeSplitX   = 2;
static constexpr auto OctreeSplitY   = 2;
static constexpr auto OctreeSplitZ   = 2;
static constexpr auto OctreeChildren = OctreeSplitX * OctreeSplitY * OctreeSplitZ;

template <typename Type>
class OctreeLeaf {
public:
    static constexpr auto IsNode = false;
    OctreeLeaf(const Component::BoundingVolume& a_Bounds = {});
    void SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max);
    template <typename Op>
    void Visit(Op& a_Op);
    template <typename Op>
    void Visit(Op& a_Op) const;
    bool Contains(const Component::BoundingVolume& a_BoundingVolume);
    bool Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume);
    void Clear();
    bool empty = true;
    Component::BoundingVolume bounds;
    std::vector<Type> storage;
};

template <typename Type, size_t Depth, size_t MaxDepth>
struct OctreeNode : OctreeLeaf<Type> {
    static_assert(MaxDepth >= 1);
    static constexpr auto IsNode = Depth < MaxDepth;
    using OctreeLeaf<Type>::OctreeLeaf;
    using LeafType     = OctreeLeaf<Type>;
    using NodeType     = OctreeNode<Type, Depth + 1, MaxDepth>;
    using ChildrenType = std::conditional<IsNode, NodeType, LeafType>::type;
    OctreeNode(const Component::BoundingVolume& a_Bounds = {});
    /**
     * @brief recalculates the Octree's bounding volumes
     */
    void SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max);
    /**
     * @brief visits the Octree, if Op returns false, stops and don't visit children
     * @tparam Op an OctreeVisitor that returns true if we should visit children and false if not
     * @param a_Op the visitor
     */
    template <typename Op>
    void Visit(Op& a_Op);
    /**
     * @brief visits the Octree, if Op returns false, stops and don't visit children
     * @tparam Op an OctreeVisitor that returns true if we should visit children and false if not
     * @param a_Op the visitor
     */
    template <typename Op>
    void Visit(Op& a_Op) const;
    bool Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume);
    /**
     * @brief clears this node and its children
     */
    void Clear();
    std::array<ChildrenType, OctreeChildren> children;
};

template <typename Type, size_t MaxDepth = 2>
using Octree = OctreeNode<Type, 0, MaxDepth>;

template <typename Type, size_t Depth, size_t MaxDepth>
inline OctreeNode<Type, Depth, MaxDepth>::OctreeNode(const Component::BoundingVolume& a_Bounds)
{
    SetMinMax(a_Bounds.Min(), a_Bounds.Max());
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline void OctreeNode<Type, Depth, MaxDepth>::SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max)
{
    this->bounds.SetMinMax(a_Min, a_Max);
    auto size = (a_Max - a_Min) / 2.f;
    for (uint8_t z = 0; z < OctreeSplitZ; z++) {
        glm::vec3 boxMin {};
        glm::vec3 boxMax {};
        boxMin.z = a_Min.z + size.z * z;
        boxMax.z = boxMin.z + size.z;
        for (uint8_t y = 0; y < OctreeSplitY; y++) {
            boxMin.y = a_Min.y + size.y * y;
            boxMax.y = boxMin.y + size.y;
            for (uint8_t x = 0; x < OctreeSplitX; x++) {
                boxMin.x   = a_Min.x + size.x * x;
                boxMax.x   = boxMin.x + size.x;
                auto index = (z * OctreeSplitX * OctreeSplitY) + (y * OctreeSplitX) + x;
                children[index].SetMinMax(boxMin, boxMax);
            }
        }
    }
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline bool OctreeNode<Type, Depth, MaxDepth>::Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume)
{
    if (this->Contains(a_BoundingVolume)) {
        this->empty = false;
        for (auto& child : children) {
            if (child.Insert(a_Val, a_BoundingVolume)) {
                return true;
            }
        }
        this->storage.push_back(a_Val);
        return true;
    }
    return false;
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline void OctreeNode<Type, Depth, MaxDepth>::Clear()
{
    if (this->empty)
        return;
    OctreeLeaf<Type>::Clear();
    for (auto& child : children)
        child.Clear();
}

template <typename Type, size_t Depth, size_t MaxDepth>
template <typename Op>
inline void OctreeNode<Type, Depth, MaxDepth>::Visit(Op& a_Op)
{
    if (!a_Op(*this))
        return;
    for (auto& child : children)
        child.Visit(a_Op);
}

template <typename Type, size_t Depth, size_t MaxDepth>
template <typename Op>
inline void OctreeNode<Type, Depth, MaxDepth>::Visit(Op& a_Op) const
{
    if (!a_Op(*this))
        return;
    for (auto& child : children)
        child.Visit(a_Op);
}

template <typename Type>
inline OctreeLeaf<Type>::OctreeLeaf(const Component::BoundingVolume& a_Bounds)
    : bounds(a_Bounds)
{
}

template <typename Type>
inline void OctreeLeaf<Type>::SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max)
{
    bounds.SetMinMax(a_Min, a_Max);
}

template <typename Type>
inline bool OctreeLeaf<Type>::Contains(const Component::BoundingVolume& a_BoundingVolume)
{
    auto thisMin = bounds.Min();
    auto thisMax = bounds.Max();
    auto boxMin  = a_BoundingVolume.Min();
    auto boxMax  = a_BoundingVolume.Max();
    return glm::all(glm::lessThanEqual(thisMin, boxMin))
        && glm::all(glm::greaterThanEqual(thisMax, boxMax));
}

template <typename Type>
inline bool OctreeLeaf<Type>::Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume)
{
    if (Contains(a_BoundingVolume)) {
        storage.push_back(a_Val);
        empty = false;
        return true;
    }
    return false;
}

template <typename Type>
inline void OctreeLeaf<Type>::Clear()
{
    empty = true;
    storage.clear();
}

template <typename Type>
template <typename Op>
inline void OctreeLeaf<Type>::Visit(Op& a_Op)
{
    a_Op(*this);
}

template <typename Type>
template <typename Op>
inline void OctreeLeaf<Type>::Visit(Op& a_Op) const
{
    a_Op(*this);
}
}
