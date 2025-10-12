#pragma once
////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
#include <MSG/ECS/ComponentTypeStorage.hpp>
#include <MSG/ECS/EntityRef.hpp>
#include <MSG/ECS/EntityStorage.hpp>
#include <MSG/ECS/View.hpp>
#include <MSG/FixedSizeMemoryPool.hpp>

#include <gcem.hpp>

#include <cassert>
#include <cstdint>
#include <limits>
#include <mutex>
#include <typeindex>
#include <unordered_map>

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace Msg::ECS {
/**
 * @brief The ECS registry, it keeps track of the entites and components
 */
template <typename EntityIDT = uint32_t, size_t MaxEntitiesV = gcem::pow(2, 17), size_t MaxComponentTypesV = gcem::pow(2, 8)>
class Registry {
public:
    static constexpr auto MaxEntities       = MaxEntitiesV;
    static constexpr auto MaxComponentTypes = MaxComponentTypesV;
    typedef EntityIDT EntityIDType;
    typedef Registry<EntityIDType, MaxEntities, MaxComponentTypes> RegistryType;
    typedef EntityRef<RegistryType> EntityRefType; /** @copydoc EntityRef * The EntityRef type used by this Registry type */
    typedef EntityStorage<RegistryType> EntityStorageType;
    typedef ComponentTypeStorageI<EntityIDType> ComponentTypeStorageType;
    friend EntityRefType;

    ~Registry()
    {
        for (const auto& entity : _entities)
            assert(entity == nullptr && "Some entities outlived the registry");
        for (auto componentStorage : _componentTypeStorage) {
            delete componentStorage.second;
        }
    }

    /** @brief the registry cannot be created on stack because of its size */
    static std::shared_ptr<Registry> Create()
    {
        return std::shared_ptr<Registry>(new Registry);
    }

    /** @return a reference to a newly created entity */
    template <typename... Components>
    [[nodiscard]] EntityRefType CreateEntity();
    /** @return a reference to the specified entity */
    [[nodiscard]] EntityRefType GetEntityRef(const EntityIDType& a_Entity);
    /** @return true if the specified entity is alive */
    bool IsAlive(const EntityIDType& a_Entity) const;
    /** @return the number of alive entities */
    size_t Count() const;
    /** @return the reference count of the specified entity */
    size_t GetEntityRefCount(const EntityIDType& a_Entity);

    /**
     * @brief Constructs a component using the arguments and attaches it to the entity
     * @return the newly created component
     */
    template <typename T, typename... Args>
    auto& AddComponent(const EntityIDType& a_Entity, Args&&... a_Args);
    /** @brief Removes the component of the specified type from the entity */
    template <typename T>
    void RemoveComponent(const EntityIDType& a_Entity);
    /** @return true if the entity has a component of the specified type */
    template <typename T>
    bool HasComponent(const EntityIDType& a_Entity) const;
    /** @return The component of the specified type */
    template <typename T>
    T& GetComponent(const EntityIDType& a_Entity);
    template <typename T>
    const T& GetComponent(const EntityIDType& a_Entity) const;
    /** @returns A View of the registery with the specified types */
    template <typename... ToGet, typename... ToExclude>
    auto GetView(Exclude<ToExclude...> = {});

    /**
     * @brief It is recomended to lock the Registry before doing multiple operations on it
     * @return this Registry's recursive mutex
     */
    auto& GetLock() { return _lock; }

private:
    Registry();
    template <typename T>
    auto& _GetStorage();
    template <typename T>
    auto& _GetStorage() const;
    void _DestroyEntity(const EntityIDType& a_Entity);

    template <typename Factory>
    struct LazyConstructor {
        using result_type = std::invoke_result_t<const Factory&>;
        constexpr LazyConstructor(Factory&& a_Factory)
            : factory(a_Factory)
        {
        }
        constexpr operator result_type() const noexcept(std::is_nothrow_invocable_v<const Factory&>)
        {
            return factory();
        }
        const Factory factory;
    };
    mutable std::recursive_mutex _lock;
    std::unordered_map<std::type_index, ComponentTypeStorageType*> _componentTypeStorage;
    std::array<EntityStorageType*, MaxEntities> _entities;
    FixedSizeMemoryPool<EntityStorageType, MaxEntities> _entityPool;
};

/** @copydoc Registry * The default Registry with default template arguments */
typedef Registry<uint32_t> DefaultRegistry;
}

#include <MSG/ECS/Registry.inl>
