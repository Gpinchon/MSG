#pragma once

#include <MSG/BoundingVolume.hpp>
#include <MSG/FixedSizeMemoryPool.hpp>

#include <sparsehash/sparse_hash_map>

#include <memory>
#include <optional>
#include <vector>

namespace Msg {
template <typename BVHType>
struct BVHNode : public std::enable_shared_from_this<BVHNode<BVHType>> {
    using bvh_type             = BVHType;
    using value_type           = bvh_type::value_type;
    using type                 = BVHNode<bvh_type>;
    using node_shared_ptr_type = std::shared_ptr<type>;
    using node_weak_ptr_type   = std::weak_ptr<type>;
    void Refit();
    void RemoveChild(bvh_type& a_BVH, const node_shared_ptr_type& a_Child);
    bool IsLeaf() const { return object.has_value(); }
    BoundingVolume bounds;
    std::optional<value_type> object;
    node_weak_ptr_type parent;
    node_shared_ptr_type child1;
    node_shared_ptr_type child2;
};

template <typename Type, uint32_t MaxSize = 65635>
class BVH {
public:
    static constexpr auto max_size = MaxSize;
    using value_type               = Type;
    using type                     = BVH<value_type>;
    using node_type                = BVHNode<type>;
    using node_shared_ptr_type     = std::shared_ptr<node_type>;
    using node_weak_ptr_type       = std::weak_ptr<node_type>;
    using sparse_hash_map_type     = google::sparse_hash_map<value_type, node_weak_ptr_type>;
    using memory_pool_type         = FixedSizeMemoryPool<node_type, max_size>;

public:
    BVH();
    bool Contains(const Type& a_Object);
    void Clear();
    void FreeNode(const node_shared_ptr_type& a_NodeIndex);
    node_shared_ptr_type AllocateNode();
    node_shared_ptr_type PickBestSibling(const node_shared_ptr_type& a_Node);
    node_shared_ptr_type GetLeafNode(const Type& a_Object);
    void MapObject(const node_shared_ptr_type& a_Node);
    void UnmapObject(const node_shared_ptr_type& a_Node);
    template <typename... Args>
    void InsertLeaf(const BoundingVolume& a_BV, Args... a_Args);
    void RemoveLeaf(const Type& a_Object);
    void Refit(const node_shared_ptr_type& a_Node);
    template <typename Op>
    void Visit(Op& a_Op) const;
    template <typename Op>
    void Visit(Op& a_Op);
    memory_pool_type memoryPool;
    node_shared_ptr_type root;
    sparse_hash_map_type objectsToNodes;
};
}

#include <MSG/BVH.inl>
