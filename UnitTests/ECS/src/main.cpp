#include <SG/Entity/Node/Group.hpp>

#include <ECS/Registry.hpp>
#include <ECS/SparseSet.hpp>

#include <Tools/ScopedTimer.hpp>

#include <string>
#include <iostream>
#include <set>
#include <glm/vec3.hpp>

using namespace TabGraph;

auto TestECS0() {
    auto registry = ECS::DefaultRegistry::Create();
    auto& mutex = registry->GetLock();
    std::scoped_lock lock(mutex);
    {
        Tools::ScopedTimer timer("Creating/destructing 1000000 entities");
        for (auto i = 0u; i < 1000000; ++i) {
            SG::Node::Group::Create(registry);
        }
    }
    std::vector<ECS::DefaultRegistry::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 10 entities and printing their name");
        for (auto i = 0u; i < 10; ++i) {
            auto entity = SG::Node::Group::Create(registry);
            entities.push_back(entity);
        }
        registry->GetView<SG::Component::Name>().ForEach<SG::Component::Name>([](auto entity, auto& name) {
            std::cout << std::string(name) << ", ";
        });
        std::cout << '\n';
    }
    entities.clear();
    {
        Tools::ScopedTimer timer("Creating " + std::to_string(ECS::DefaultRegistry::MaxEntities) + " entities");
        for (auto i = 0u; i < ECS::DefaultRegistry::MaxEntities; ++i) {
            auto entity = registry->CreateEntity();
            entity.AddComponent<SG::Component::Transform>();
            if (i % 2) entity.AddComponent<SG::Component::Name>();
            entities.push_back(entity);
        }
    }
    {
        Tools::ScopedTimer timer("Updating positions");
        registry->GetView<SG::Component::Transform>().ForEach<SG::Component::Transform>([](auto entity, auto& transform) {
            transform.SetPosition({ entity, 0, 0 });
        });
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        registry->GetView<SG::Component::Transform>().ForEach<SG::Component::Transform>([](auto entity, auto& transform) {
            assert(transform.GetPosition().x == entity);
        });
    }
    {
        Tools::ScopedTimer timer("Checking components");
        size_t entityCount = 0;
        registry->GetView<SG::Component::Name>().ForEach<SG::Component::Name>([&entityCount](auto entity, auto& name) {
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

void TestECS1()
{
    auto registry = ECS::DefaultRegistry::Create();
    auto& mutex = registry->GetLock();
    std::scoped_lock lock(mutex);
    std::set<ECS::DefaultRegistry::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 1000 nodes and setting parenting");
        auto entity = SG::Node::Group::Create(registry);
        entities.insert(entity);
        {
            auto lastEntity(entity);
            for (auto i = 0u; i < 999; ++i) {
                auto newEntity = SG::Node::Group::Create(registry);
                lastEntity.GetComponent<SG::Component::Children>().insert(newEntity);
                newEntity.GetComponent<SG::Component::Transform>().GetPosition().x = i;
                lastEntity = newEntity;
            }
        }

    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform");
        registry->GetView<SG::Component::Transform>().ForEach<SG::Component::Transform>(
        [&nodeCount](auto, auto&) {
            nodeCount++;
        });
    }
    assert(nodeCount == 1000);
    std::cout << "Node Count : " << nodeCount << std::endl; //should get 100 entities
    {
        Tools::ScopedTimer timer("Removing root node");
        entities.clear(); //remove root node
    }
    nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform again");
        registry->GetView<SG::Component::Transform>().ForEach<SG::Component::Transform>([&nodeCount](auto, auto&) {
            nodeCount++;
        });
    }
    assert(nodeCount == 0);
    std::cout << "Node Count : " << nodeCount << std::endl; //should get 0 entity
}

struct Test {
    Test() = delete;
    Test(const std::string& a_V) : v(a_V) {};
    std::string v;
};

void TestSparseSet()
{
    auto nameSet = new ECS::SparseSet<Test, 65535>;
    for (auto i = 0u; i < 1000; ++i) {
        nameSet->insert(i, std::to_string(i));
    }
    for (auto i = 0u; i < 1000; ++i) {
        if (i % 3) nameSet->erase(i);
    }
    for (auto i = 0u; i < 1000; ++i) {
        if (i % 3) assert(!nameSet->contains(i));
        else assert(nameSet->contains(i));
    }
    delete nameSet;
}

int main() {
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("TestSparseSet");
        TestSparseSet();
    }
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
    int v;
    std::cin >> v;
    return 0;
}