#pragma once
////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <limits>
#ifndef NDEBUG
#include <cassert>
#endif

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG::ECS {
/**
 * @brief Wraps a reference to an entity,
 * maintains reference count and destroys entity when released
 */
template <typename RegistryType>
class EntityRef {
public:
    using IDType                    = typename RegistryType::EntityIDType;
    static constexpr auto DefaultID = std::numeric_limits<IDType>::max();

    EntityRef() = default;
    EntityRef(const EntityRef& a_Other);
    EntityRef(EntityRef&& a_Other) noexcept;
    ~EntityRef();
    bool Empty() const;
    template <typename T, typename... Args>
    T& AddComponent(Args&&... a_Args) const;
    template <typename T>
    bool HasComponent() const;
    template <typename T>
    T& GetComponent() const;
    template <typename T>
    void RemoveComponent() const;
    void Reset();
    IDType GetID() const;
    RegistryType* GetRegistry() const;
    uint32_t RefCount() const;
    operator IDType() const;
    EntityRef& operator=(const EntityRef& a_Other);

    // comparison operators have to be defined inside class

    friend bool operator<(const EntityRef& a_Left, const EntityRef& a_Right) { return a_Left._id < a_Right._id; }
    friend bool operator>(const EntityRef& a_Left, const EntityRef& a_Right) { return a_Left._id > a_Right._id; }
    friend bool operator!=(const EntityRef& a_Left, const EntityRef& a_Right) { return a_Left < a_Right || a_Left > a_Right; }
    friend bool operator==(const EntityRef& a_Left, const EntityRef& a_Right) { return !(a_Left != a_Right); }

private:
    friend RegistryType;
    void Ref();
    void Unref();
    EntityRef(IDType a_ID, RegistryType* a_Registry, uint32_t* a_RefCount);
    IDType _id { DefaultID };
    RegistryType* _registry { nullptr };
    uint32_t* _refCount { nullptr };
};
}

#include <MSG/ECS/EntityRef.inl>
