#include <ECS/Registry.hpp>

#include <Tools/ScopedTimer.hpp>

#include <iostream>
#include <set>
#include <glm/vec3.hpp>

using namespace TabGraph;

class Name {
public:
    Name(const std::string a_Value = "") : _name(a_Value) {}
    operator std::string() { return _name; }

private:
    std::string _name;
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
    std::set<ECS::Registry<>::EntityRefType> children;
};

template<typename RegistryType>
auto CreateNode(const RegistryType& a_Registry) {
    auto entity = a_Registry->CreateEntity();
    entity.AddComponent<Transform>();
    entity.AddComponent<Parent>();
    return entity;
}

template<typename RegistryType>
auto CreateNodeGroup(const RegistryType& a_Registry) {
    auto entity = CreateNode(a_Registry);
    entity.AddComponent<Children>();
    return entity;
}

template<typename Registry>
auto TestECS(const Registry& a_Registry) {
    std::scoped_lock lock(a_Registry->GetLock());
    {
        Tools::ScopedTimer timer("Creating/destructing 1.000.000 entities");
        for (auto i = 0u; i < 1000000; ++i)
        {
            auto entity = a_Registry->CreateEntity();
            entity.AddComponent<Transform>();
            entity.AddComponent<Name>();
            auto view = a_Registry->GetView<Transform, Name>();
            auto& transform = entity.GetComponent<Transform>();
            transform.position.x += 10;
            entity.RemoveComponent<Transform>();
        }
    }
    
    std::vector<ECS::Registry<>::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 60000 entities");
        for (auto i = 0u; i < 60000; ++i) {
            auto entity = a_Registry->CreateEntity();
            entity.AddComponent<Transform>();
            if (i % 2) entity.AddComponent<Name>();
            entities.push_back(entity);
        }
    }
    auto view = a_Registry->GetView<Transform, Name>();
    {
        Tools::ScopedTimer timer("Updating positions");
        view.ForEach<Transform>([a_Registry](auto entity, auto& transform) {
            transform.position.x = entity;
        });
    }
    {
        Tools::ScopedTimer timer("Checking components");
        size_t entityCount = 0;
        view.ForEach<Name>([a_Registry, &entityCount](auto entity, auto& name) {
            entityCount++;
        });
        assert(entityCount == 30000);
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        view.ForEach<Transform>([a_Registry](auto entity, Transform& transform) {
            assert(transform.position.x == entity);
        });
    }
    {
        Tools::ScopedTimer timer("Deleting entities");
        for (auto i = 0u; i < entities.size(); ++i) {
            if (i % 3) entities.at(i) = {};
        }
    }
    {
        Tools::ScopedTimer timer("Clearing remaining entities");
        entities.clear();
    }
}

struct Scene {
    std::set<ECS::Registry<>::EntityRefType> entities;
};

int main() {
    TestECS(ECS::Registry<>::Create());
    auto registry = ECS::Registry<>::Create();
    {
        Scene scene;
        std::scoped_lock lock(registry->GetLock());
        auto entity = CreateNodeGroup(registry);
        scene.entities.insert(entity);
        {
            auto lastEntity = entity;
            for (auto i = 0u; i < 10; ++i) {
                auto newEntity = CreateNodeGroup(registry);
                auto& newTransform = newEntity.GetComponent<Transform>();
                newTransform.position.x = i;
                scene.entities.insert(newEntity);
                lastEntity = newEntity;
            }
        }
        size_t nodeCount = 0;
        registry->GetView<Transform>().ForEach<Transform>([&nodeCount](auto entity, auto& transform) {
            nodeCount++;
        });
        std::cout << "Node Count : " << nodeCount << std::endl; //should get 11 entities
        scene.entities.erase(entity);
    }
    size_t nodeCount = 0;
    registry->GetView<Transform>().ForEach<Transform>([&nodeCount](auto entity, auto& transform) {
        nodeCount++;
    });
    std::cout << "Node Count : " << nodeCount << std::endl; //should get 0 entity
    
}