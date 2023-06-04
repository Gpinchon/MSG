#include <SG/Entity/NodeGroup.hpp>

#include <SG/Component/Name.hpp>

#include <ECS/Registry.hpp>

#include <Tools/ScopedTimer.hpp>
#include <Tools/SparseSet.hpp>

#include <glm/vec3.hpp>
#include <iostream>
#include <set>
#include <string>

using namespace TabGraph;

auto TestECS0()
{
    auto registry = ECS::DefaultRegistry::Create();
    auto& mutex   = registry->GetLock();
    std::scoped_lock lock(mutex);
    {
        Tools::ScopedTimer timer("Creating/destructing 1000000 entities");
        for (auto i = 0u; i < 1000000; ++i) {
            SG::NodeGroup::Create(registry);
        }
    }
    std::vector<ECS::DefaultRegistry::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 10 entities and printing their name");
        for (auto i = 0u; i < 10; ++i) {
            auto entity = SG::NodeGroup::Create(registry);
            entities.push_back(entity);
        }
        registry->GetView<SG::Component::Name>().ForEach<SG::Component::Name>([](auto& name) {
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
            if (i % 2)
                entity.AddComponent<SG::Component::Name>();
            entities.push_back(entity);
        }
    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform but without name");
        registry->GetView<SG::Component::Transform>(ECS::Exclude<SG::Component::Name>()).ForEach<SG::Component::Transform>([&nodeCount](auto, auto&) {
            nodeCount++;
        });
    }
    assert(nodeCount == ECS::DefaultRegistry::MaxEntities / 2);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 100 entities
    {
        Tools::ScopedTimer timer("Updating positions");
        registry->GetView<SG::Component::Transform>().ForEach<SG::Component::Transform>([](auto entity, auto& transform) {
            transform.position.x = entity;
        });
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        registry->GetView<SG::Component::Transform>().ForEach<SG::Component::Transform>([](auto entity, auto& transform) {
            assert(transform.position.x == entity);
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
        Tools::ScopedTimer timer("Deleting " + std::to_string(size_t(2 / 3.f * entities.size())) + " entities");
        for (auto i = 0u; i < entities.size(); ++i) {
            if (i % 3)
                entities.at(i) = {};
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
    auto& mutex   = registry->GetLock();
    std::scoped_lock lock(mutex);
    std::set<ECS::DefaultRegistry::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 900 nodes and setting parenting");
        auto entity = SG::NodeGroup::Create(registry);
        entities.insert(entity);
        {
            auto lastEntity(entity);
            for (auto i = 0u; i < 899; ++i) {
                auto newEntity = SG::NodeGroup::Create(registry);
                lastEntity.GetComponent<SG::Component::Children>().insert(newEntity);
                newEntity.GetComponent<SG::Component::Transform>().position.x = i;
                lastEntity                                                    = newEntity;
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
    assert(nodeCount == 900);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 900 entities
    {
        Tools::ScopedTimer timer("Removing root node");
        entities.clear(); // remove root node
    }
    nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform again");
        registry->GetView<SG::Component::Transform>().ForEach<SG::Component::Transform>([&nodeCount](auto, auto&) {
            nodeCount++;
        });
    }
    assert(nodeCount == 0);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 0 entity
}

void TestSparseSet()
{
    auto sparseSet = new Tools::SparseSet<SG::Component::Transform, gcem::pow(2, 17)>;
    for (auto i = 0u; i < sparseSet->max_size(); ++i) {
        sparseSet->insert(i).position.x = i;
    }
    for (auto i = 0u; i < sparseSet->size(); ++i) {
        assert(sparseSet->at(i).position.x == i);
    }
    for (auto i = 0u; i < sparseSet->max_size(); ++i) {
        if (i % 3)
            sparseSet->erase(i);
    }
    for (auto i = 0u; i < sparseSet->max_size(); ++i) {
        if (i % 3)
            assert(!sparseSet->contains(i));
        else
            assert(sparseSet->contains(i));
    }
    delete sparseSet;
}

int main()
{
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
