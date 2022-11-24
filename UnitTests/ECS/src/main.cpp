#include <ECS/Manager.hpp>

#include <glm/vec3.hpp>

#include <vector>
#include <iostream>

using namespace TabGraph;

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