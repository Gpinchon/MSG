#include <SG/Node/NodeGroup.hpp>

#include <ECS/Registry.hpp>

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
            SG::CreateNodeGroup(registry);
        }
    }
    std::vector<ECS::DefaultRegistry::EntityRefType> entities;
    {
        Tools::ScopedTimer timer("Creating 10 entities and printing their name");
        for (auto i = 0u; i < 10; ++i) {
            auto entity = SG::CreateNodeGroup(registry);
            entities.push_back(entity);
        }
        registry->GetView<SG::Name>().ForEach<SG::Name>([](auto entity, auto& name) {
            std::cout << std::string(name) << ", ";
        });
        std::cout << '\n';
    }
    entities.clear();
    {
        Tools::ScopedTimer timer("Creating " + std::to_string(ECS::DefaultRegistry::MaxEntities) + " entities");
        for (auto i = 0u; i < ECS::DefaultRegistry::MaxEntities; ++i) {
            auto entity = registry->CreateEntity();
            entity.AddComponent<SG::Transform>();
            if (i % 2) entity.AddComponent<SG::Name>();
            entities.push_back(entity);
        }
    }
    {
        Tools::ScopedTimer timer("Updating positions");
        registry->GetView<SG::Transform>().ForEach<SG::Transform>([](auto entity, auto& transform) {
            transform.position.x = entity;
        });
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        registry->GetView<SG::Transform>().ForEach<SG::Transform>([](auto entity, auto& transform) {
            assert(transform.position.x == entity);
        });
    }
    {
        Tools::ScopedTimer timer("Checking components");
        size_t entityCount = 0;
        registry->GetView<SG::Name>().ForEach<SG::Name>([&entityCount](auto entity, auto& name) {
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
        auto entity = SG::CreateNodeGroup(registry);
        entities.insert(entity);
        {
            auto lastEntity(entity);
            for (auto i = 0u; i < 999; ++i) {
                auto newEntity = SG::CreateNodeGroup(registry);
                lastEntity.GetComponent<SG::Children>().insert(newEntity);
                newEntity.GetComponent<SG::Transform>().position.x = i;
                lastEntity = newEntity;
            }
        }

    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform");
        registry->GetView<SG::Transform>().ForEach<SG::Transform>(
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
        registry->GetView<SG::Transform>().ForEach<SG::Transform>([&nodeCount](auto, auto&) {
            nodeCount++;
        });
    }
    assert(nodeCount == 0);
    std::cout << "Node Count : " << nodeCount << std::endl; //should get 0 entity
}

#include <SG/Node/NodeGroup.hpp>
#include <SG/Visitor/SearchVisitor.hpp>

auto TestOOSG0() {
    {
        Tools::ScopedTimer timer("Creating/destructing 1000000 entities");
        for (auto i = 0u; i < 1000000; ++i) {
            auto entity = std::make_shared<SG::NodeGroup>();
        }
    }
    std::vector<std::shared_ptr<SG::NodeGroup>> entities;
    {
        Tools::ScopedTimer timer("Creating 10 entities and printing their name");
        for (auto i = 0u; i < 10; ++i) {
            auto entity = std::make_shared<SG::NodeGroup>();
            entities.push_back(entity);
        }
        SG::SearchVisitor search(typeid(SG::Object), SG::NodeVisitor::Mode::VisitChildren);
        for (auto& node : entities) node->Accept(search);
        for (const auto& obj : search.GetResult())
            std::cout << std::string(obj->GetName()) << ", ";
        std::cout << '\n';
    }
    entities.clear();
    {
        Tools::ScopedTimer timer("Creating " + std::to_string(ECS::DefaultRegistry::MaxEntities) + " entities");
        for (auto i = 0u; i < ECS::DefaultRegistry::MaxEntities; ++i) {
            auto entity = std::make_shared<SG::NodeGroup>();
            entities.push_back(entity);
        }
    }
    {
        Tools::ScopedTimer timer("Updating positions");
        SG::SearchVisitor search(typeid(SG::Object), SG::NodeVisitor::Mode::VisitChildren);
        for (auto& node : entities) node->Accept(search);
        for (auto& node : search.GetResult()) ((SG::Node*)node)->SetLocalPosition({ node->GetId(), 0, 0 });
    }
    {
        Tools::ScopedTimer timer("Checking positions");
        SG::SearchVisitor search(typeid(SG::Object), SG::NodeVisitor::Mode::VisitChildren);
        for (auto& node : entities) node->Accept(search);
        for (auto& node : search.GetResult()) assert(((SG::Node*)node)->GetLocalPosition().x == node->GetId());
    }
    {
        Tools::ScopedTimer timer("Deleting " + std::to_string(size_t(2 / 3.f * entities.size())) + " entities");
        for (auto i = 0u; i < entities.size(); ++i) {
            if (i % 3) entities.at(i) = {};
        }
    }
    {
        Tools::ScopedTimer timer("Clearing remaining entities");
        entities.clear();
    }
}

void TestOOSG1() {
    std::vector<std::shared_ptr<SG::NodeGroup>> nodes;
    {
        Tools::ScopedTimer timer("Creating 1000 nodes and setting parenting");
        auto node = std::make_shared<SG::NodeGroup>();
        nodes.push_back(node);
        auto lastNode(node);
        for (auto i = 0u; i < 999; ++i) {
            auto newNode = std::make_shared<SG::NodeGroup>();
            newNode->SetParent(lastNode);
            newNode->SetLocalPosition({ i, 0, 0 });
            lastNode = newNode;
        }
    }
    size_t nodeCount = 0;
    {
        Tools::ScopedTimer timer("Counting nodes with transform");
        SG::SearchVisitor search(typeid(SG::Node), SG::NodeVisitor::Mode::VisitChildren);
        (*nodes.begin())->Accept(search);
        for (const auto& obj : search.GetResult())
            nodeCount++;
    }
    assert(nodeCount == 1000);
    std::cout << "Node Count : " << nodeCount << std::endl; //should get 100 entities
    {
        Tools::ScopedTimer timer("Removing root node");
        nodes.clear();
    }
}

int main() {
    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Testing with ECS\n";
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
    std::cout << "Testing with OO SG\n";
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("TestOOSG0");
        TestOOSG0();
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("TestOOSG1");
        TestOOSG1();
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    int v;
    std::cin >> v;
    return 0;
}