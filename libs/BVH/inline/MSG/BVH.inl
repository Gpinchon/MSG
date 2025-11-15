#pragma once

namespace Msg {
template <typename BVHType>
inline void Msg::BVHNode<BVHType>::RemoveChild(bvh_type& a_BVH, const node_shared_ptr_type& a_Child)
{
    node_shared_ptr_type nodeToKeep;
    if (child1 == a_Child) {
        nodeToKeep = child2;
    } else if (child2 == a_Child) {
        nodeToKeep = child1;
    } else {
        throw std::runtime_error("The node is not the child of this nod");
    }
    // adopt the node's data except parent
    bounds = nodeToKeep->bounds;
    object = nodeToKeep->object;
    child1 = nodeToKeep->child1;
    child2 = nodeToKeep->child2;
    if (IsLeaf())
        a_BVH.MapObject(this->shared_from_this());
    else {
        child1->parent = this->shared_from_this();
        child2->parent = this->shared_from_this();
    }
    Refit();
}

template <typename BVHType>
inline void Msg::BVHNode<BVHType>::Refit()
{
    // Walk back up the tree refitting AABBs
    for (auto node = parent.lock(); node != nullptr; node = node->parent.lock()) {
        auto newBounds = node->child1->bounds + node->child2->bounds;
        if (newBounds != node->bounds)
            node->bounds = newBounds;
        else // no need to go up further !
            break;
    }
}

template <typename Type, uint32_t MaxSize>
inline BVH<Type, MaxSize>::BVH()
{
    value_type deletedValue;
    std::memset(&deletedValue, 255, sizeof(value_type));
    objectsToNodes.set_deleted_key(std::hash<value_type> {}(deletedValue));
}

template <typename Type, uint32_t MaxSize>
inline bool BVH<Type, MaxSize>::Contains(const Type& a_Object)
{
    auto itr = objectsToNodes.find(a_Object);
    return itr != objectsToNodes.end();
}

template <typename Type, uint32_t MaxSize>
inline void BVH<Type, MaxSize>::Clear()
{
    root.reset();
    objectsToNodes.clear();
}
template <typename Type, uint32_t MaxSize>
inline auto BVH<Type, MaxSize>::AllocateNode() -> node_shared_ptr_type
{
    // return std::make_shared<node_type>();
    auto ptr = memoryPool.allocate();
    std::construct_at(ptr);
    return node_shared_ptr_type(ptr, memoryPool.deleter());
}
template <typename Type, uint32_t MaxSize>
inline void BVH<Type, MaxSize>::FreeNode(const node_shared_ptr_type& a_Node)
{
    if (a_Node->IsLeaf())
        UnmapObject(a_Node);
    if (!a_Node->parent.expired()) {
        auto parent = a_Node->parent.lock();
        parent->RemoveChild(*this, a_Node);
    } else {
        assert(root == a_Node);
        root.reset();
    }
}
template <typename Type, uint32_t MaxSize>
inline auto BVH<Type, MaxSize>::PickBestSibling(const node_shared_ptr_type& a_Node) -> node_shared_ptr_type
{
    node_shared_ptr_type bestSibling;
    float newNodeArea = a_Node->bounds.Area();
    float bestCost    = std::numeric_limits<float>::max();
    struct Candidate {
        node_shared_ptr_type node;
        float inheritedCost;
    };
    std::vector<Candidate> stack;
    stack.emplace_back(Candidate { root, 0.f });
    while (!stack.empty()) {
        auto currentNode   = stack.back().node;
        auto inheritedCost = stack.back().inheritedCost;
        auto combinedBV    = currentNode->bounds + a_Node->bounds;
        auto directCost    = combinedBV.Area();
        auto cost          = directCost + inheritedCost;
        stack.pop_back();
        if (cost < bestCost) {
            bestCost    = cost;
            bestSibling = currentNode;
        }
        inheritedCost += directCost - currentNode->bounds.Area();
        if ((newNodeArea + inheritedCost) < bestCost && !currentNode->IsLeaf()) {
            stack.emplace_back(Candidate { currentNode->child1, inheritedCost });
            stack.emplace_back(Candidate { currentNode->child2, inheritedCost });
        }
    }
    assert(bestSibling != nullptr);
    return bestSibling;
}
template <typename Type, uint32_t MaxSize>
inline auto BVH<Type, MaxSize>::GetLeafNode(const Type& a_Object) -> node_shared_ptr_type
{
    auto itr = objectsToNodes.find(a_Object);
    if (itr == objectsToNodes.end())
        return nullptr;
    return itr->second.lock();
}
template <typename Type, uint32_t MaxSize>
inline void BVH<Type, MaxSize>::MapObject(const node_shared_ptr_type& a_Node)
{
    objectsToNodes[*a_Node->object] = a_Node;
}
template <typename Type, uint32_t MaxSize>
inline void BVH<Type, MaxSize>::UnmapObject(const node_shared_ptr_type& a_Node)
{
    auto itrOtN = objectsToNodes.find(*a_Node->object);
    assert(itrOtN != objectsToNodes.end());
    objectsToNodes.erase(itrOtN);
}
template <typename Type, uint32_t MaxSize>
template <typename... Args>
void BVH<Type, MaxSize>::InsertLeaf(const BoundingVolume& a_BV, Args... a_Args)
{
    auto leaf = AllocateNode();
    leaf->object.emplace(std::forward<Args>(a_Args)...);
    leaf->bounds = a_BV;
    MapObject(leaf);
    if (root == nullptr) {
        root = leaf;
        return;
    }
    auto sibling = PickBestSibling(leaf);
    // Create a new parent
    auto oldParent = sibling->parent.lock();
    auto newParent = AllocateNode();
    // Connect new leaf and sibling to new parent
    newParent->bounds = a_BV + sibling->bounds;
    newParent->parent = oldParent;
    newParent->child1 = sibling;
    newParent->child2 = leaf;
    sibling->parent   = newParent;
    leaf->parent      = newParent;
    if (oldParent != nullptr) {
        // The sibling was not the root
        if (oldParent->child1 == sibling)
            oldParent->child1 = newParent;
        else
            oldParent->child2 = newParent;
    } else // The sibling was the root
        root = newParent;
    Refit(leaf);
}
template <typename Type, uint32_t MaxSize>
inline void BVH<Type, MaxSize>::RemoveLeaf(const Type& a_Object)
{
    auto removeLeaf = GetLeafNode(a_Object);
    assert(removeLeaf->IsLeaf());
    FreeNode(removeLeaf);
    return;
    auto parentNode = removeLeaf->parent.lock();
    if (parentNode != nullptr) {
        node_shared_ptr_type keepLeaf;
        if (parentNode->child1 == removeLeaf)
            keepLeaf = parentNode->child2;
        else if (parentNode->child2 == removeLeaf)
            keepLeaf = parentNode->child1;
        else
            throw std::runtime_error("This leaf is not the child of its parent, WTF");
        auto parentParent = parentNode->parent.lock();
        keepLeaf->parent  = parentParent;
        if (parentParent != nullptr) {
            if (parentParent->child1 == parentNode)
                parentParent->child1 = keepLeaf;
            else if (parentParent->child2 == parentNode)
                parentParent->child2 = keepLeaf;
            else
                throw std::runtime_error("The parent is not the child of its parent, WTF");
        }
        Refit(keepLeaf);
    }
}
template <typename Type, uint32_t MaxSize>
inline void BVH<Type, MaxSize>::Refit(const node_shared_ptr_type& a_Leaf)
{
    // Walk back up the tree refitting AABBs
    for (auto node = a_Leaf->parent.lock(); node != nullptr; node = node->parent.lock()) {
        node->bounds = node->child1->bounds + node->child2->bounds;
    }
}
template <typename Type, uint32_t MaxSize>
template <typename Op>
void BVH<Type, MaxSize>::Visit(Op& a_Op) const
{
    std::vector<node_shared_ptr_type> stack;
    stack.emplace_back(root);
    while (!stack.empty()) {
        auto currentNode = stack.back();
        stack.pop_back();
        if (a_Op(*this, *currentNode) && !currentNode->IsLeaf()) {
            stack.emplace_back(currentNode->child1);
            stack.emplace_back(currentNode->child2);
        }
    }
}
template <typename Type, uint32_t MaxSize>
template <typename Op>
void BVH<Type, MaxSize>::Visit(Op& a_Op)
{
    std::vector<BVHNode*> stack;
    stack.emplace_back(root);
    while (!stack.empty()) {
        auto currentNode = stack.back();
        stack.pop_back();
        if (a_Op(*this, *currentNode) && !currentNode->IsLeaf()) {
            stack.emplace_back(currentNode->child1);
            stack.emplace_back(currentNode->child2);
        }
    }
}
}