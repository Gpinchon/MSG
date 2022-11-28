#include <ECS/Registry.hpp>

#include <Tools/ScopedTimer.hpp>

#include <string>
#include <iostream>
#include <set>
#include <glm/vec3.hpp>

using namespace TabGraph;

struct Name {
    Name() = default;
    Name(std::string& a_Value) : _value(a_Value) {}
    operator std::string& () {
        return _value;
    }
private:
    std::string _value;
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
    template<typename T>
    void insert(const T& a_Entity) { _entities.insert(a_Entity); }
    template<typename T>
    void erase(const T& a_Entity)  { _entities.erase(a_Entity); }
    auto begin() { return _entities.begin(); }
    auto end()   { return _entities.end(); }

private:
    std::set<ECS::DefaultRegistry::EntityRefType> _entities;
};

template<typename RegistryType>
auto CreateObject(const RegistryType& a_Registry) {
    static auto s_ObjectNbr = 0u;
    auto entity = a_Registry->CreateEntity();
    entity.AddComponent<Name>("Object_" + std::to_string(++s_ObjectNbr));
    return entity;
}

template<typename RegistryType>
auto CreateNode(const RegistryType& a_Registry) {
    static auto s_NodeNbr = 0u;
    auto entity = CreateObject(a_Registry);
    entity.GetComponent<Name>() = "Node_" + std::to_string(++s_NodeNbr);
    entity.AddComponent<Transform>();
    entity.AddComponent<Parent>();
    return entity;
}

template<typename RegistryType>
auto CreateNodeGroup(const RegistryType& a_Registry) {
    static auto s_NodeGroupNbr = 0u;
    auto entity = CreateNode(a_Registry);
    entity.GetComponent<Name>() = "NodeGroup_" + std::to_string(++s_NodeGroupNbr);
    entity.AddComponent<Children>();
    return entity;
}

auto TestECS0() {
    auto registry = ECS::DefaultRegistry::Create();
    std::scoped_lock lock(registry->GetLock());
    {
        Tools::ScopedTimer timer("Creating/destructing 1000000 entities");
        for (auto i = 0u; i < 1000000; ++i) {
            auto entity = CreateNodeGroup(registry);
        }
    }
    std::vector<ECS::DefaultRegistry::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 10 entities and printing their name");
        for (auto i = 0u; i < 10; ++i) {
            auto entity = CreateNodeGroup(registry);
            entities.push_back(entity);
        }
        registry->GetView<Name>().ForEach<Name>([](auto entity, auto& name) {
            std::cout << std::string(name) << ", ";
        });
        std::cout << std::endl;
        entities.clear();
    }
    {
        Tools::ScopedTimer timer("Creating " + std::to_string(ECS::DefaultRegistry::MaxEntities) + " entities");
        for (auto i = 0u; i < ECS::DefaultRegistry::MaxEntities; ++i) {
            auto entity = registry->CreateEntity();
            entity.AddComponent<Transform>();
            if (i % 2) entity.AddComponent<Name>();
            entities.push_back(entity);
        }
    }
    auto view = registry->GetView<Transform, Name>();
    {
        Tools::ScopedTimer timer("Updating positions");
        view.ForEach<Transform>([](auto entity, auto& transform) {
            transform.position.x = entity;
        });
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        view.ForEach<Transform>([](auto entity, Transform& transform) {
            assert(transform.position.x == entity);
        });
    }
    {
        Tools::ScopedTimer timer("Checking components");
        size_t entityCount = 0;
        view.ForEach<Name>([&entityCount](auto entity, auto& name) {
            entityCount++;
        });
        assert(entityCount == ECS::DefaultRegistry::MaxEntities / 2);
    }
    {
        Tools::ScopedTimer timer("Deleting " + std::to_string(size_t(2/3.f * entities.size())) + " entities");
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
    std::set<ECS::DefaultRegistry::EntityRefType> entities;
};

void TestECS1()
{
    auto registry = ECS::DefaultRegistry::Create();
    Scene scene;
    {
        std::scoped_lock lock(registry->GetLock());
        Tools::ScopedTimer timer("Creating 100 nodes and setting parenting");
        auto entity = CreateNodeGroup(registry);
        scene.entities.insert(entity);
        {
            auto lastEntity = entity;
            for (auto i = 0u; i < 99; ++i) {
                auto newEntity = CreateNodeGroup(registry);
                auto& newTransform = newEntity.GetComponent<Transform>();
                lastEntity.GetComponent<Children>().insert(newEntity);
                newTransform.position.x = i;
                lastEntity = newEntity;
            }
        }

    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform");

        registry->GetView<Transform>().ForEach<Transform>([&nodeCount](auto entity, auto& transform) {
            nodeCount++;
            });
        assert(nodeCount == 100);
    }
    std::cout << "Node Count : " << nodeCount << std::endl; //should get 100 entities
    {
        Tools::ScopedTimer timer("Removing root node");
        scene.entities.clear(); //remove root node
    }
    nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform again");
        registry->GetView<Transform>().ForEach<Transform>([&nodeCount](auto entity, auto& transform) {
            nodeCount++;
            });
        assert(nodeCount == 0);
    }
    std::cout << "Node Count : " << nodeCount << std::endl; //should get 0 entity
}

int main() {
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("TestECS0");
        TestECS0();
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("TestECS1");
        TestECS1();
    }
    std::cout << "--------------------------------------------------------------------------------\n";
}