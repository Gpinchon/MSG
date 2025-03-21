#include <MSG/Core/Name.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/NodeGroup.hpp>
#include <MSG/SparseSet.hpp>
#include <MSG/Tools/ScopedTimer.hpp>

#include <glm/vec3.hpp>
#include <iostream>
#include <set>
#include <string>

#include <gtest/gtest.h>

using namespace MSG;

TEST(ECS, Test0)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto& mutex   = registry->GetLock();
    std::scoped_lock lock(mutex);
    {
        Tools::ScopedTimer timer("Creating/destructing 1000000 entities");
        for (auto i = 0u; i < 1000000; ++i) {
            Entity::NodeGroup::Create(registry);
        }
    }
    std::vector<ECS::DefaultRegistry::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 10 entities and printing their name");
        for (auto i = 0u; i < 10; ++i) {
            auto entity = Entity::NodeGroup::Create(registry);
            entities.push_back(entity);
        }
        registry->GetView<Core::Name>().ForEach([](auto entityID, auto& name) {
            std::cout << std::string(name) << ", ";
        });
        std::cout << '\n';
    }
    entities.clear();
    {
        Tools::ScopedTimer timer("Creating " + std::to_string(ECS::DefaultRegistry::MaxEntities) + " entities");
        for (auto i = 0u; i < ECS::DefaultRegistry::MaxEntities; ++i) {
            auto entity = registry->CreateEntity();
            entity.template AddComponent<MSG::Transform>();
            if (i % 2)
                entity.template AddComponent<Core::Name>();
            entities.push_back(entity);
        }
    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform but without name");
        registry->GetView<MSG::Transform>(ECS::Exclude<Core::Name>()).ForEach([&nodeCount](auto) {
            nodeCount++;
        });
    }
    ASSERT_EQ(nodeCount, ECS::DefaultRegistry::MaxEntities / 2);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 100 entities
    {
        Tools::ScopedTimer timer("Updating positions");
        registry->GetView<MSG::Transform>().ForEach([](auto entity, auto& transform) {
            transform.SetLocalPosition({ entity, 0, 0 });
        });
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        registry->GetView<MSG::Transform>().ForEach([](auto entity, auto& transform) {
            ASSERT_EQ(transform.GetLocalPosition().x, entity);
        });
    }
    {
        Tools::ScopedTimer timer("Checking components");
        size_t entityCount = 0;
        registry->GetView<Core::Name>().ForEach([&entityCount](auto entity) {
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
        auto entity = Entity::NodeGroup::Create(registry);
        entities.insert(entity);
        {
            auto lastEntity(entity);
            for (auto i = 0u; i < 899; ++i) {
                auto newEntity = Entity::NodeGroup::Create(registry);
                lastEntity.template GetComponent<Children>().insert(newEntity);
                newEntity.template GetComponent<MSG::Transform>().SetLocalPosition({ i, 0, 0 });
                lastEntity = newEntity;
            }
        }
    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform");
        registry->GetView<MSG::Transform>().ForEach(
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
        registry->GetView<MSG::Transform>().ForEach([&nodeCount](auto) {
            nodeCount++;
        });
    }
    ASSERT_EQ(nodeCount, 0);
    std::cout << "Node Count : " << nodeCount << std::endl; // should get 0 entity
}

TEST(ECS, SparseSet)
{
    auto sparseSet = new SparseSet<MSG::Transform, gcem::pow(2, 17)>;
    for (auto i = 0u; i < sparseSet->max_size(); ++i) {
        sparseSet->insert(i).SetLocalPosition({ i, 0, 0 });
    }
    for (auto i = 0u; i < sparseSet->size(); ++i) {
        ASSERT_EQ(sparseSet->at(i).GetLocalPosition().x, i);
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
