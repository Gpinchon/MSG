#include <ECS/Manager.hpp>

#include <glm/vec3.hpp>

#include <vector>
#include <iostream>

using namespace TabGraph;

#include <memory_resource>
#include <random>
#include <unordered_map>
#include <functional>

template<typename Type, size_t Size>
class MemoryPool {
public:
    class Deleter {
    public:
        Deleter(MemoryPool& a_Pool) : _memoryPool(a_Pool) {}
        void operator()(Type* const a_Ptr) { _memoryPool.deallocate(a_Ptr, 1); }

    private :
        MemoryPool& _memoryPool;
    };
    using value_type = Type;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using delete_functor = std::function<void(Type*)>;

    MemoryPool(delete_functor a_CallBack = {}) : _deleteCallback(a_CallBack) {}

    Type* allocate(size_t a_Count) noexcept {
        ++_count;
        return _allocator.allocate(a_Count);
    }
    void deallocate(Type* const a_Ptr, const size_t a_Count) noexcept {
        if (_deleteCallback) _deleteCallback(a_Ptr);
        --_count;
        return _allocator.deallocate(a_Ptr, a_Count);
    }
    auto deleter() noexcept {
        return Deleter(*this);
    }
    auto size() const noexcept {
        return Size;
    }
    auto count() const noexcept {
        return _count;
    }
    auto free() const noexcept {
        return Size - _count;
    }

private:
    size_t _count{ 0 };
    std::byte _memory[Size];
    std::pmr::monotonic_buffer_resource _mbr{ _memory, sizeof(_memory) };
    std::pmr::polymorphic_allocator<Type> _allocator{ &_mbr };
    const delete_functor _deleteCallback;
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

template<typename EntityType = uint16_t, size_t MaxEntities = std::numeric_limits<EntityType>::max()>
class Registry {
public:
    auto CreateEntity() {
        return std::shared_ptr<EntityType>(new(_entityPool.allocate(1)) EntityType(_idGenerator()), _entityPool.deleter());
    }
    template<typename T, typename... Args>
    auto AddComponent(EntityType a_Entity, Args... a_Args) {
        const std::type_index typeID = typeid(T);
        auto& allocator = _componentPools[typeID];
        if (allocator == nullptr) allocator = std::make_shared<MemoryPool<T, MaxEntities>>();
        auto objPool = std::reinterpret_pointer_cast<MemoryPool<T, MaxEntities>>(allocator);
        auto component = std::shared_ptr<T>(new(objPool->allocate(1)) T(a_Args...), objPool->deleter());
        _entityComponents[a_Entity].insert(Component(component));
        return component;
    }
    template<typename T>
    void RemoveComponent(EntityType a_Entity) {
        _entityComponents.at(a_Entity).erase(Component(typeid(T)));
    }

private:
    void EntityDeleted(EntityType* a_EntityPtr) {
        _entityComponents.erase(*a_EntityPtr);
    }
    IDGenerator<EntityType> _idGenerator;
    MemoryPool<EntityType, MaxEntities> _entityPool{ std::bind(&Registry::EntityDeleted, this, std::placeholders::_1) };
    std::unordered_map<std::type_index, std::shared_ptr<void>> _componentPools;
    std::unordered_map<EntityType, std::set<Component>> _entityComponents;
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
    Registry registry;
    auto entity = registry.CreateEntity();
    registry.AddComponent<Transform>(*entity);
    registry.RemoveComponent<Transform>(*entity);

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