#include <SG/Entity/NodeGroup.hpp>

#include <SG/Component/Name.hpp>

#include <ECS/Registry.hpp>

#include <Tools/ScopedTimer.hpp>
#include <Tools/SparseSet.hpp>

#include <glm/vec3.hpp>
#include <iostream>
#include <set>
#include <string>

#include <gtest/gtest.h>

using namespace TabGraph;

TEST(ECS, Test0)
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
        registry->GetView<SG::Component::Name>().ForEach([](auto entityID, auto& name) {
            std::cout << std::string(name) << ", ";
        });
        std::cout << '\n';
    }
    entities.clear();
    {
        Tools::ScopedTimer timer("Creating " + std::to_string(ECS::DefaultRegistry::MaxEntities) + " entities");
        for (auto i = 0u; i < ECS::DefaultRegistry::MaxEntities; ++i) {
            auto entity = registry->CreateEntity();
            entity.template AddComponent<SG::Component::Transform>();
            if (i % 2)
                entity.template AddComponent<SG::Component::Name>();
            entities.push_back(entity);
        }
    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform but without name");
        registry->GetView<SG::Component::Transform>(ECS::Exclude<SG::Component::Name>()).ForEach([&nodeCount](auto) {
            nodeCount++;
        });
    }
    ASSERT_EQ(nodeCount, ECS::DefaultRegistry::MaxEntities / 2);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 100 entities
    {
        Tools::ScopedTimer timer("Updating positions");
        registry->GetView<SG::Component::Transform>().ForEach([](auto entity, auto& transform) {
            transform.position.x = entity;
        });
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        registry->GetView<SG::Component::Transform>().ForEach([](auto entity, auto& transform) {
            ASSERT_EQ(transform.position.x, entity);
        });
    }
    {
        Tools::ScopedTimer timer("Checking components");
        size_t entityCount = 0;
        registry->GetView<SG::Component::Name>().ForEach([&entityCount](auto entity) {
            entityCount++;
        });
        ASSERT_EQ(entityCount, ECS::DefaultRegistry::MaxEntities / 2);
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

TEST(ECS, Test1)
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
                lastEntity.template GetComponent<SG::Component::Children>().insert(newEntity);
                newEntity.template GetComponent<SG::Component::Transform>().position.x = i;
                lastEntity                                                    = newEntity;
            }
        }
    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform");
        registry->GetView<SG::Component::Transform>().ForEach(
            [&nodeCount](auto, auto&) {
                nodeCount++;
            });
    }
    ASSERT_EQ(nodeCount, 900);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 900 entities
    {
        Tools::ScopedTimer timer("Removing root node");
        entities.clear(); // remove root node
    }
    nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform again");
        registry->GetView<SG::Component::Transform>().ForEach([&nodeCount](auto) {
            nodeCount++;
        });
    }
    ASSERT_EQ(nodeCount, 0);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 0 entity
}

TEST(ECS, SparseSet)
{
    auto sparseSet = new Tools::SparseSet<SG::Component::Transform, gcem::pow(2, 17)>;
    for (auto i = 0u; i < sparseSet->max_size(); ++i) {
        sparseSet->insert(i).position.x = i;
    }
    for (auto i = 0u; i < sparseSet->size(); ++i) {
        ASSERT_EQ(sparseSet->at(i).position.x, i);
    }
    for (auto i = 0u; i < sparseSet->max_size(); ++i) {
        if (i % 3)
            sparseSet->erase(i);
    }
    for (auto i = 0u; i < sparseSet->max_size(); ++i) {
        if (i % 3)
            ASSERT_FALSE(sparseSet->contains(i));
        else
            ASSERT_TRUE(sparseSet->contains(i));
    }
    delete sparseSet;
}



int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
