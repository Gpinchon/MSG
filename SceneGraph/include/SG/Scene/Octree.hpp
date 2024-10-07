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
#include <utility>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
static constexpr auto OctreeSplitX   = 2;
static constexpr auto OctreeSplitY   = 2;
static constexpr auto OctreeSplitZ   = 2;
static constexpr auto OctreeChildren = OctreeSplitX * OctreeSplitY * OctreeSplitZ;

template <size_t MaxDepth>
class OctreeRef : public std::array<uint8_t, MaxDepth> {
public:
    OctreeRef() { this->fill(OctreeChildren); }
    bool Empty(const size_t& a_Index) const { return this->at(a_Index) >= OctreeChildren; }
};

template <typename Type>
class OctreeLeaf {
public:
    static constexpr auto IsNode = false;

    OctreeLeaf(const Component::BoundingVolume& a_Bounds = {});
    const Component::BoundingVolume& Bounds() const;
    const std::vector<Type>& Storage() const;
    void SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max);
    template <typename Op>
    void Visit(Op& a_Op);
    template <typename Op>
    void Visit(Op& a_Op) const;
    bool Contains(const Component::BoundingVolume& a_BoundingVolume) const;
    bool Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume);
    void Clear();
    bool Empty() const { return _storage.empty(); }
    size_t Size() const;

protected:
    Component::BoundingVolume _bounds;
    std::vector<Type> _storage;
};

template <typename Type, size_t TDepth, size_t MaxDepth>
class OctreeNode : public OctreeLeaf<Type> {
public:
    static_assert(MaxDepth >= 1);
    static constexpr auto Depth  = TDepth;
    static constexpr auto IsNode = Depth < MaxDepth; /// @brief is true if this node has children
    using OctreeLeaf<Type>::OctreeLeaf;
    using LeafType     = OctreeLeaf<Type>;
    using NodeType     = OctreeNode<Type, Depth + 1, MaxDepth>;
    using ChildrenType = std::conditional<IsNode, NodeType, LeafType>::type;
    using RefType      = OctreeRef<MaxDepth>;

    /**
     * @brief builds an empty node, initializing its children
     * @param a_Bounds : the bounds of this tree
     */
    OctreeNode(const Component::BoundingVolume& a_Bounds = {});
    /**
     * @brief recalculates this node's bounding volumes, updates children
     */
    void SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max);
    /**
     * @brief visits this node, if Op returns false, stops and don't visit children
     * @tparam Op : bool(Node&) Node can be OctreeLeaf or OctreeNode
     * @param a_Op : the visitor
     */
    template <typename Op>
    void Visit(Op& a_Op);
    /**
     * @brief visits this node, if Op returns false, stops and don't visit children
     * @tparam Op : bool(Node&) Node can be OctreeLeaf or OctreeNode
     * @param a_Op : the visitor
     */
    template <typename Op>
    void Visit(Op& a_Op) const;
    /**
     * @brief attempts to insert a new element into this node,
     * if it fits it will go down the node's hierarchy to try and find a tighter fit
     * @param a_Val : the new element to insert
     * @param a_BoundingVolume : the bounding volume of this element
     * @return true if insertion was successful, false otherwise
     */
    bool Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume);
    /**
     * @brief attempts to insert a new element into this node,
     * if it fits it will go down the node's hierarchy to try and find a tighter fit
     * @param a_At : where to start insertion
     * @param a_Val : the new element to insert
     * @param a_BoundingVolume : the bounding volume of this element
     * @return a pair where first = true if insertion is successful and second = where this value was inserted
     */
    std::pair<bool, RefType> Insert(const RefType& a_At, const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume);
    /**
     * @brief clears this node and its children
     */
    void Clear();
    /**
     * @return the octree size starting from this node
     */
    size_t Size() const;
    /**
     * @return true if the node and its children are empty
     */
    bool Empty() const
    {
        return Size() == 0;
    }

private:
    size_t _UpdateSize();
    size_t _childrenSize = 0;
    std::array<ChildrenType, OctreeChildren> _children;
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
    OctreeLeaf<Type>::SetMinMax(a_Min, a_Max);
    auto size        = (a_Max - a_Min) / 2.f;
    glm::vec3 boxMin = {};
    glm::vec3 boxMax = {};
    for (uint8_t z = 0; z < OctreeSplitZ; z++) {
        boxMin.z = a_Min.z + size.z * z;
        boxMax.z = boxMin.z + size.z;
        for (uint8_t y = 0; y < OctreeSplitY; y++) {
            boxMin.y = a_Min.y + size.y * y;
            boxMax.y = boxMin.y + size.y;
            for (uint8_t x = 0; x < OctreeSplitX; x++) {
                boxMin.x   = a_Min.x + size.x * x;
                boxMax.x   = boxMin.x + size.x;
                auto index = (z * OctreeSplitX * OctreeSplitY) + (y * OctreeSplitX) + x;
                _children[index].SetMinMax(boxMin, boxMax);
            }
        }
    }
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline bool OctreeNode<Type, Depth, MaxDepth>::Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume)
{
    if (this->Contains(a_BoundingVolume)) {
        for (auto& child : _children) {
            if (child.Insert(a_Val, a_BoundingVolume)) {
                this->_childrenSize++;
                return true;
            }
        }
        this->_storage.push_back(a_Val);
        return true;
    }
    return false;
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline auto OctreeNode<Type, Depth, MaxDepth>::Insert(const RefType& a_At, const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume) -> std::pair<bool, RefType>
{
    if (!a_At.Empty(Depth)) {
        auto& child = _children.at(a_At.at(Depth));
        std::pair<bool, RefType> result;
        if constexpr (ChildrenType::IsNode) {
            result = child.Insert(a_At, a_Val, a_BoundingVolume);
        } else {
            result = { child.Insert(a_Val, a_BoundingVolume), a_At };
        }
        if (result.first)
            this->_childrenSize++;
        return result;
    } else if (this->Contains(a_BoundingVolume)) {
        RefType ref = a_At;
        for (ref.at(Depth) = 0; ref.at(Depth) < OctreeChildren; ref.at(Depth)++) {
            auto& child = _children.at(ref.at(Depth));
            std::pair<bool, RefType> result;
            if constexpr (ChildrenType::IsNode) {
                result = child.Insert(ref, a_Val, a_BoundingVolume);
            } else {
                result = { child.Insert(a_Val, a_BoundingVolume), ref };
            }
            if (result.first) {
                this->_childrenSize++;
                return result;
            }
        }
        this->_storage.push_back(a_Val);
        return { true, a_At };
    }
    return { false, {} };
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline void OctreeNode<Type, Depth, MaxDepth>::Clear()
{
    if (Empty())
        return;
    OctreeLeaf<Type>::Clear();
    for (auto& child : _children)
        child.Clear();
    this->_childrenSize = 0;
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline size_t OctreeNode<Type, Depth, MaxDepth>::Size() const
{
    return OctreeLeaf<Type>::Size() + this->_childrenSize;
}

template <typename Type, size_t Depth, size_t MaxDepth>
template <typename Op>
inline void OctreeNode<Type, Depth, MaxDepth>::Visit(Op& a_Op)
{
    if (!a_Op(*this))
        return;
    this->_childrenSize = 0;
    for (auto& child : _children) {
        child.Visit(a_Op);
        this->_childrenSize += child.Size();
    }
}

template <typename Type, size_t Depth, size_t MaxDepth>
template <typename Op>
inline void OctreeNode<Type, Depth, MaxDepth>::Visit(Op& a_Op) const
{
    if (!a_Op(*this))
        return;
    for (auto& child : _children)
        child.Visit(a_Op);
}

template <typename Type, size_t Depth, size_t MaxDepth>
inline size_t OctreeNode<Type, Depth, MaxDepth>::_UpdateSize()
{
    _childrenSize = 0;
    for (auto& child : _children)
        _childrenSize += child._UpdateSize();
    return _childrenSize;
}

template <typename Type>
inline OctreeLeaf<Type>::OctreeLeaf(const Component::BoundingVolume& a_Bounds)
    : _bounds(a_Bounds)
{
}

template <typename Type>
inline const Component::BoundingVolume& OctreeLeaf<Type>::Bounds() const
{
    return _bounds;
}

template <typename Type>
inline const std::vector<Type>& OctreeLeaf<Type>::Storage() const
{
    return _storage;
}

template <typename Type>
inline void OctreeLeaf<Type>::SetMinMax(const glm::vec3& a_Min, const glm::vec3& a_Max)
{
    _bounds.SetMinMax(a_Min, a_Max);
}

template <typename Type>
inline bool OctreeLeaf<Type>::Contains(const Component::BoundingVolume& a_BoundingVolume) const
{
    auto thisMin = _bounds.Min();
    auto thisMax = _bounds.Max();
    auto boxMin  = a_BoundingVolume.Min();
    auto boxMax  = a_BoundingVolume.Max();
    return glm::all(glm::lessThanEqual(thisMin, boxMin))
        && glm::all(glm::greaterThanEqual(thisMax, boxMax));
}

template <typename Type>
inline bool OctreeLeaf<Type>::Insert(const Type& a_Val, const Component::BoundingVolume& a_BoundingVolume)
{
    if (Contains(a_BoundingVolume)) {
        _storage.push_back(a_Val);
        return true;
    }
    return false;
}

template <typename Type>
inline void OctreeLeaf<Type>::Clear()
{
    _storage.clear();
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

template <typename Type>
inline size_t OctreeLeaf<Type>::Size() const
{
    return _storage.size();
}
}
