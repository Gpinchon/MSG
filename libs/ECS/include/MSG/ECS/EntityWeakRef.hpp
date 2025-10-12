#pragma once
////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <cstdint>
#include <limits>
#ifndef NDEBUG
#include <cassert>
#endif

#include <MSG/ECS/EntityRef.hpp>
#include <MSG/ECS/Registry.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg::ECS {
/**
 * @brief Wraps a reference to an entity,
 * does not maintain ref count and does not destroy entity when released
 */
template <typename RegistryType>
class EntityWeakRef {
public:
    inline EntityWeakRef() = default;
    inline EntityWeakRef(const EntityWeakRef& a_Other)
    {
        *this = a_Other;
    }
    inline EntityWeakRef(EntityWeakRef&& a_Other) noexcept
    {
        std::swap(_id, a_Other._id);
        std::swap(_registry, a_Other._registry);
    }
    inline EntityWeakRef(EntityRef<RegistryType>& a_Ref)
        : _id(a_Ref._id)
        , _registry(a_Ref._registry)
    {
    }

    void Reset() { *this = EntityWeakRef {}; }

    bool Empty() const { return _registry == nullptr; }

    bool Expired() const { return Empty() || !_registry->IsAlive(_id); }

    auto Lock() const { return _registry->GetEntityRef(_id); }

    EntityWeakRef& operator=(const EntityRef<RegistryType>& a_Other)
    {
        _id       = a_Other.GetID();
        _registry = a_Other.GetRegistry();
        return *this;
    }

    EntityWeakRef& operator=(const EntityWeakRef& a_Other) = default;

private:
    EntityRef<RegistryType>::IDType _id { EntityRef<RegistryType>::DefaultID };
    RegistryType* _registry { nullptr };
};
}
