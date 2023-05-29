#pragma once
////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#ifdef _DEBUG
#include <cassert>
#endif

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::ECS {
/**
 * @brief Wraps a reference to an entity,
 * maintains reference count and destroys entity when released
 */
template <typename RegistryType>
class EntityRef {
public:
    typedef typename RegistryType::EntityIDType IDType;
    static constexpr auto DefaultID = std::numeric_limits<IDType>::max();

    inline EntityRef() = default;
    inline EntityRef(const EntityRef& a_Other)
    {
        *this = a_Other;
    }
    inline EntityRef(EntityRef&& a_Other) noexcept
    {
        std::swap(_id, a_Other._id);
        std::swap(_registry, a_Other._registry);
        std::swap(_refCount, a_Other._refCount);
    }
    inline ~EntityRef()
    {
        Unref();
    }
    template <typename T, typename... Args>
    inline auto& AddComponent(Args&&... a_Args) const
    {
        return _registry->AddComponent<T>(_id, std::forward<Args>(a_Args)...);
    }
    template <typename T>
    inline bool HasComponent() const
    {
        return _registry->HasComponent<T>(_id);
    }
    template <typename T>
    inline auto& GetComponent() const
    {
        return _registry->GetComponent<T>(_id);
    }
    template <typename T>
    inline void RemoveComponent() const
    {
        return _registry->RemoveComponent<T>(_id);
    }

    auto GetRegistry() const { return _registry; }

    auto RefCount() const { return _refCount == nullptr ? 0 : *_refCount; }

    operator IDType() const { return _id; }

    EntityRef& operator=(const EntityRef& a_Other)
    {
        Unref();
        _id       = a_Other._id;
        _registry = a_Other._registry;
        _refCount = a_Other._refCount;
        Ref();
        return *this;
    }

    friend bool operator<(const EntityRef& a_Left, const EntityRef& a_Right)
    {
        return a_Left._id < a_Right._id;
    }
    friend bool operator>(const EntityRef& a_Left, const EntityRef& a_Right)
    {
        return a_Left._id > a_Right._id;
    }
    friend bool operator!=(const EntityRef& a_Left, const EntityRef& a_Right)
    {
        return a_Left < a_Right || a_Left > a_Right;
    }
    friend bool operator==(const EntityRef& a_Left, const EntityRef& a_Right)
    {
        return !(a_Left != a_Right);
    }

private:
    friend RegistryType;
    void Ref()
    {
        if (_refCount != nullptr)
            (*_refCount)++;
    }
    void Unref()
    {
        if (_refCount == nullptr)
            return; // empty ref
#ifdef _DEBUG
        assert((*_refCount) > 0); // Entity already destroyed
#endif
        (*_refCount)--;
        if (*_refCount == 0) {
            _registry->_DestroyEntity(_id);
            _refCount = nullptr;
            _registry = nullptr;
        }
    }
    inline EntityRef(IDType a_ID, RegistryType* a_Registry, uint32_t* a_RefCount)
        : _id(a_ID)
        , _registry(a_Registry)
        , _refCount(a_RefCount)
    {
        (*_refCount)++;
    }
    IDType _id { DefaultID };
    RegistryType* _registry { nullptr };
    uint32_t* _refCount { nullptr };
};
}
