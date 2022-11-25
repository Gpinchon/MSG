#include <ECS/Manager.hpp>

#include <glm/vec3.hpp>

#include <vector>
#include <iostream>

using namespace TabGraph;

#include <memory_resource>
#include <random>
#include <unordered_map>
#include <functional>

template<typename Type, size_t Size = 4096>
class MemoryPool {
public:
    class Deleter {
    public:
        Deleter(MemoryPool& a_Pool) : _memoryPool(a_Pool) {}
        void operator()(Type* const a_Ptr) { _memoryPool.deallocate(a_Ptr, 1); }

    private :
        MemoryPool& _memoryPool;
    };
    typedef Type value_type;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    template<typename U> struct rebind { typedef MemoryPool<U, Size> other; };

    MemoryPool() = default;
    MemoryPool(MemoryPool&& a_Other)
        : _count(std::move(a_Other._count))
        , _memory(std::move(a_Other._memory))
        , _mbr(std::move(a_Other._mbr))
    {}
    MemoryPool(const MemoryPool& a_Other) noexcept {}
    template<typename U>
    MemoryPool(const MemoryPool<U, Size>&) noexcept {}

    Type* allocate(size_t a_Count) noexcept {
        ++_count;
        return static_cast<value_type*>(_mbr.allocate(sizeof(value_type) * a_Count));
    }
    void deallocate(Type* const a_Ptr, const size_t a_Count) noexcept {
        --_count;
        return _mbr.deallocate(a_Ptr, sizeof(value_type) * a_Count);
    }
    auto deleter() noexcept {
        return Deleter(*this);
    }
    auto max_size() const noexcept {
        return Size;
    }
    auto count() const noexcept {
        return _count;
    }
    auto free() const noexcept {
        return Size - _count;
    }

    template<typename U>
    bool operator!=(const MemoryPool<U, Size>& a_Right) { return false; }
    template<typename U>
    bool operator==(const  MemoryPool<U, Size>& a_Right) { return !(*this != a_Right); }

private:
    size_t _count{ 0 };
    std::vector<std::byte> _memory{ sizeof(Type) * Size };
    std::pmr::monotonic_buffer_resource _mbr{ _memory.data(), _memory.size() };
};

template<typename T>
class IDGenerator {
public:
    auto operator()() {
        return _dist(_gen);
    }
private:
    std::random_device _rd;
    std::mt19937 _gen{ _rd() };
    std::uniform_int_distribution<T> _dist{ 0, std::numeric_limits<T>::max() };
};

class Component {
public:
    template<typename T>
    Component(const std::shared_ptr<T>& a_Ptr)
        : _type_index(typeid(T))
        , _ptr(a_Ptr)
    {}
    Component(const std::type_index& a_typeIndex)
        : _type_index(a_typeIndex)
    {}

    friend bool operator<(const Component& a_Left, const Component& a_Right) {
        return a_Left._type_index < a_Right._type_index;
    }
    friend bool operator>(const Component& a_Left, const Component& a_Right) {
        return a_Left._type_index > a_Right._type_index;
    }
    friend bool operator!=(const Component& a_Left, const Component& a_Right) {
        return a_Left < a_Right || a_Left > a_Right;
    }
    friend bool operator==(const Component& a_Left, const Component& a_Right) {
        return !(a_Left != a_Right);
    }
private:
    const std::type_index _type_index;
    const std::shared_ptr<void> _ptr;
};

template<typename EntityType, typename RegistryType>
class Entity {
public:
    Entity(const Entity& a_Other) : Entity(a_Other._id, a_Other._registry) {}
    Entity(Entity&& a_Other) {
        std::swap(_id, a_Other._id);
        std::swap(_registry, a_Other._registry);
    }
    ~Entity() { _registry.Unref(_id); }
    operator EntityType() { return _id; }

private:
    friend RegistryType;
    Entity(EntityType a_ID, RegistryType& a_Registry)
        : _id(a_ID)
        , _registry(a_Registry)
    {
        _registry.Ref(_id);
    }
    
    EntityType _id;
    RegistryType& _registry;
};

template<typename Key, typename Type, size_t MaxSize, template<typename, size_t> typename Allocator>
class PooledMap {
public:
    auto& at(Key a_Key) {
        return *_storage.at(a_Key);
    }
    template<typename... Args>
    void insert(Key a_Key, Args... a_Args) {
        _storage.insert(std::pair(a_Key, new(_allocator.allocate(1)) Type(a_Args...)));
    }
    void erase(Key a_Key) {
        auto& storage = _storage.at(a_Key);
        std::destroy_at(storage);
        _allocator.deallocate(storage, 1);
        _storage.erase(a_Key);
    }
    auto& operator[](Key a_Key) {
        auto find = _storage.find(a_Key);
        if (find != _storage.end()) return *find->second;
        auto value = new(_allocator.allocate(1)) Type();
        _storage.insert(std::pair(a_Key, value));
        return *value;
    }

private:
    Allocator<Type, MaxSize>   _allocator;
    std::unordered_map<Key, Type*>  _storage;
};

template<typename EntityType = uint16_t, size_t MaxEntities = std::numeric_limits<EntityType>::max(), size_t MaxComponentTypes = 256>
class Registry {
public:
    typedef Registry<EntityType, MaxEntities> RegistryType;

    Entity<EntityType, RegistryType> CreateEntity() {
        const auto entityID = _idGenerator();
        _entities.insert(entityID);
        return { entityID, *this };
    }
    template<typename T, typename... Args>
    auto AddComponent(EntityType a_Entity, Args... a_Args) {
        auto objPool = RegisterComponent<T>();
        auto component = std::shared_ptr<T>(new(objPool->allocate(1)) T(a_Args...), objPool->deleter());
        _entities.at(a_Entity).components.insert(Component(component));
        return component;
    }
    template<typename T>
    void RemoveComponent(EntityType a_Entity) {
        _entities.at(a_Entity).components.erase(Component(typeid(T)));
    }

private:
    template<typename T>
    auto RegisterComponent() {
        const std::type_index typeID = typeid(T);
        auto& allocator = _componentPools[typeID];
        if (allocator == nullptr) allocator = std::make_shared<MemoryPool<T, MaxEntities>>();
        return std::reinterpret_pointer_cast<MemoryPool<T, MaxEntities>>(allocator);
    }
    struct EntityStorage {
        EntityStorage() = default;
        std::set<Component> components;
        size_t              refCount{ 0 };
    };
    friend Entity<EntityType, RegistryType>;
    void Ref(EntityType a_Entity) {
        auto& storage = _entities.at(a_Entity);
        ++storage.refCount;
    }
    void Unref(EntityType a_Entity) {
        auto& storage = _entities.at(a_Entity);
        assert(storage.refCount > 0);
        --storage.refCount;
        if (storage.refCount == 0) {
            _entities.erase(a_Entity);
        }
    }

    PooledMap<EntityType, EntityStorage, MaxEntities, MemoryPool> _entities;
    PooledMap<std::type_index, std::shared_ptr<void>, MaxComponentTypes, MemoryPool> _componentPools;
    IDGenerator<EntityType> _idGenerator;
};

class Name {
public:
    Name(const std::string a_Value = "") : _name(a_Value) {}
    operator std::string() { return _name; }

private:
    std::string _name;
};

class Object {
public :
    template<typename... Args>
    static auto Create(Args... a_Args) {
        return std::shared_ptr<Object>(new Object(a_Args...));
    }
    
    template<typename T, typename... Args>
    auto AddComponent(Args... a_Args) const {
        ECS::Manager::RegisterComponent<T>();
        return ECS::Manager::AddComponent<T>(_entity, a_Args...);
    }
    template<typename T>
    void RemoveComponent() const {
        return ECS::Manager::RemoveComponent<T>(_entity);
    }
    template<typename T>
    auto GetComponent() const {
        return ECS::Manager::GetComponent<T>(_entity);
    }

    ~Object() { ECS::Manager::DestroyEntity(_entity); }
    
protected:
    Object() : _entity(ECS::Manager::CreateEntity()) {
        AddComponent<Name>();
    }
    Object(const std::string& a_Name) : Object() {
        *GetComponent<Name>() = a_Name;
    }

private:
    ECS::Entity _entity;
};

struct Transform {
    glm::vec3 position;
};

class Parent {
public:
    Parent(const std::shared_ptr<Transform>& a_Parent = nullptr) { _parent = a_Parent; }
    bool expired() { return _parent.expired(); }

private:
    std::weak_ptr<Transform> _parent;
};

struct Children {
    std::set<std::shared_ptr<Transform>> children;
};

//class Node : public Object {
//public:
//    template<typename... Args>
//    static auto Create(Args... a_Args) {
//        return std::shared_ptr<Node>(new Node(a_Args...));
//    }
//protected:
//    Node() : Object() {
//        AddComponent<Transform>();
//        AddComponent<Parent>();
//    }
//    Node(const std::string& a_Name) : Node() {
//        *GetComponent<Name>() = a_Name;
//    }
//};
//
//class NodeGroup : public Node {
//public:
//    template<typename... Args>
//    static auto Create(Args... a_Args) {
//        return std::shared_ptr<Node>(new Node(a_Args...));
//    }
//protected:
//    NodeGroup() : Node() {
//        AddComponent<Children>();
//    }
//    NodeGroup(const std::string& a_Name) : Node() {
//        *GetComponent<Name>() = a_Name;
//    }
//};
//
//class Scene : public NodeGroup {
//public:
//    template<typename... Args>
//    static auto Create(Args... a_Args) {
//        return std::shared_ptr<Scene>(new Scene(a_Args...));
//    }
//
//private:
//    Scene() : NodeGroup() { RemoveComponent<Parent>(); }
//};

struct CleanupSystem : ECS::System {
    static auto Create() {
        auto system = ECS::Manager::RegisterSystem<CleanupSystem>();
        ECS::Signature signature;
        ECS::Manager::RegisterComponent<Parent>();
        signature.set(ECS::Manager::GetComponentType<Parent>());
        ECS::Manager::SetSystemSignature<CleanupSystem>(signature);
        return system;
    }
    void Update() {
        bool cleaned = false;
        do {
            std::set<ECS::Entity> toCleanup;
            for (auto const& entity : entities) {
                auto& parent = ECS::Manager::GetComponent<Parent>(entity);
                if (parent->expired()) toCleanup.insert(entity);
            }
            cleaned = !toCleanup.empty();
            for (const auto& entity : toCleanup) ECS::Manager::DestroyEntity(entity);
        } while (cleaned);
    }
};

class Scene : public Object {
public:
    static auto Create() {
        return std::shared_ptr<Scene>(new Scene);
    }
    std::set<std::shared_ptr<Object>> objects;
private:
    Scene() : Object() {
        AddComponent<Transform>();
        AddComponent<Children>();
    };
};

template<typename... Args>
auto CreateNode(Args... a_Args) {
    auto obj = Object::Create(a_Args...);
    obj->AddComponent<Transform>();
    obj->AddComponent<Parent>();
    return obj;
}

template<typename... Args>
auto CreateNodeGroup(Args... a_Args) {
    auto obj = CreateNode(a_Args...);
    obj->AddComponent<Children>();
    return obj;
}

int main() {
    auto registry = new Registry;
    {
        auto entity = registry->CreateEntity();
        registry->AddComponent<Transform>(entity);
        registry->RemoveComponent<Transform>(entity);
    }
    delete registry;

    auto cleanupSystem = CleanupSystem::Create();
    auto scene = Scene::Create();
    {
        auto entity = CreateNodeGroup();
        std::shared_ptr<Object> objToRemove;
        scene->objects.insert(entity);
        *entity->GetComponent<Parent>() = Parent(scene->GetComponent<Transform>());
        scene->GetComponent<Children>()->children.insert(entity->GetComponent<Transform>());
        {
            auto lastEntity = entity;
            for (auto i = 0u; i < 10; ++i) {
                auto newEntity = CreateNodeGroup();
                if (i == 1) objToRemove = newEntity;
                auto newTransform = newEntity->GetComponent<Transform>();
                newTransform->position.x = i;
                lastEntity->GetComponent<Children>()->children.insert(newTransform);
                *newEntity->GetComponent<Parent>() = lastEntity->GetComponent<Transform>();
                scene->objects.insert(newEntity);
                lastEntity = newEntity;
            }
        }
        //should get 11 entities
        cleanupSystem->Update();
        //scene->objects.erase(entity);
        scene->GetComponent<Children>()->children.erase(entity->GetComponent<Transform>());
        *entity->GetComponent<Parent>() = Parent(nullptr);
        cleanupSystem->Update();
    }
    //should get 0 entity
    cleanupSystem->Update();
}