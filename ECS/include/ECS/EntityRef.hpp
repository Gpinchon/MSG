#pragma once
////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <cassert>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
/**
* @brief Wraps a reference to an entity,
* maintains reference count and destroys entity when released
*/
template<typename RegistryType>
class EntityRef {
public:
    typedef typename RegistryType::EntityIDType IDType;
    inline EntityRef(const EntityRef& a_Other)
        : EntityRef(a_Other._id, a_Other._registry, a_Other._refCount)
    {}
    inline EntityRef(EntityRef&& a_Other)
        : _id(std::move(a_Other._id))
        , _registry(std::move(a_Other._registry))
        , _refCount(std::move(a_Other._refCount))
    {}
    inline ~EntityRef() {
        if (_refCount == nullptr) return; //empty ref
        assert(_refCount > 0);
        (*_refCount)--;
        if (*_refCount == 0) {
            _registry->_DestroyEntity(_id);
        }
    }
    template<typename T, typename... Args>
    inline auto& AddComponent(Args&&... a_Args) const {
        return _registry->AddComponent<T>(_id, std::forward<Args>(a_Args)...);
    }
    template<typename T>
    inline bool HasComponent() const {
        return _registry->HasComponent<T>(_id);
    }
    template<typename T>
    inline auto& GetComponent() const {
        return _registry->GetComponent<T>(_id);
    }
    template<typename T>
    inline void RemoveComponent() const {
        return _registry->RemoveComponent<T>(_id);
    }

    operator IDType() { return _id; }

    EntityRef& operator=(const EntityRef& a_Other) {
        _id = a_Other._id;
        _registry = a_Other._registry;
        _refCount = a_Other._refCount;
        (*_refCount)++;
        return *this;
    }

    friend bool operator<(const EntityRef& a_Left, const EntityRef& a_Right) {
        return a_Left._id < a_Right._id;
    }
    friend bool operator>(const EntityRef& a_Left, const EntityRef& a_Right) {
        return a_Left._id > a_Right._id;
    }
    friend bool operator!=(const EntityRef& a_Left, const EntityRef& a_Right) {
        return a_Left < a_Right || a_Left > a_Right;
    }
    friend bool operator==(const EntityRef& a_Left, const EntityRef& a_Right) {
        return !(a_Left != a_Right);
    }

private:
    friend RegistryType;
    inline EntityRef(IDType a_ID, RegistryType* a_Registry, uint32_t* a_RefCount)
        : _id(a_ID)
        , _registry(a_Registry)
        , _refCount(a_RefCount)
    {
        (*_refCount)++;
    }
    IDType          _id;
    RegistryType*   _registry{ nullptr };
    uint32_t*       _refCount{ nullptr };
};
}