#pragma once
namespace Msg::ECS {
template <typename RegistryType>
inline EntityRef<RegistryType>::EntityRef(const EntityRef& a_Other)
{
    *this = a_Other;
}
template <typename RegistryType>
inline EntityRef<RegistryType>::EntityRef(EntityRef&& a_Other) noexcept
    : _id(a_Other._id)
    , _registry(a_Other._registry)
    , _refCount(a_Other._refCount)
{
    a_Other._id       = DefaultID;
    a_Other._registry = nullptr;
    a_Other._refCount = nullptr;
}

template <typename RegistryType>
inline EntityRef<RegistryType>::~EntityRef()
{
    Unref();
}
template <typename RegistryType>
bool EntityRef<RegistryType>::Empty() const
{
    return _refCount == nullptr;
}
template <typename RegistryType>
template <typename T, typename... Args>
inline T& EntityRef<RegistryType>::AddComponent(Args&&... a_Args) const
{
    return _registry->template AddComponent<T>(_id, std::forward<Args>(a_Args)...);
}
template <typename RegistryType>
template <typename T>
inline bool EntityRef<RegistryType>::HasComponent() const
{
    return _registry->template HasComponent<T>(_id);
}
template <typename RegistryType>
template <typename T>
inline T& EntityRef<RegistryType>::GetComponent() const
{
    return _registry->template GetComponent<T>(_id);
}
template <typename RegistryType>
template <typename T>
inline void EntityRef<RegistryType>::RemoveComponent() const
{
    return _registry->template RemoveComponent<T>(_id);
}
template <typename RegistryType>
void EntityRef<RegistryType>::Reset() { *this = {}; }
template <typename RegistryType>
auto EntityRef<RegistryType>::GetID() const -> IDType { return _id; }
template <typename RegistryType>
auto EntityRef<RegistryType>::GetRegistry() const -> RegistryType* { return _registry; }
template <typename RegistryType>
uint32_t EntityRef<RegistryType>::RefCount() const { return _refCount == nullptr ? 0 : *_refCount; }
template <typename RegistryType>
EntityRef<RegistryType>::operator EntityRef<RegistryType>::IDType() const { return _id; }
template <typename RegistryType>
EntityRef<RegistryType>& EntityRef<RegistryType>::operator=(const EntityRef& a_Other)
{
    Unref();
    _id       = a_Other._id;
    _registry = a_Other._registry;
    _refCount = a_Other._refCount;
    Ref();
    return *this;
}
// private methods
template <typename RegistryType>
void EntityRef<RegistryType>::Ref()
{
    if (_refCount != nullptr)
        (*_refCount)++;
}
template <typename RegistryType>
void EntityRef<RegistryType>::Unref()
{
    if (_refCount == nullptr)
        return; // empty ref
    assert((*_refCount) > 0); // Entity already destroyed
    (*_refCount)--;
    if (*_refCount == 0) {
        _registry->_DestroyEntity(_id);
        _refCount = nullptr;
        _registry = nullptr;
    }
}
template <typename RegistryType>
inline EntityRef<RegistryType>::EntityRef(IDType a_ID, RegistryType* a_Registry, uint32_t* a_RefCount)
    : _id(a_ID)
    , _registry(a_Registry)
    , _refCount(a_RefCount)
{
    (*_refCount)++;
}
}
