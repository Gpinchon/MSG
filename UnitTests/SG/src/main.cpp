#include <SG/Entity/Node.hpp>
#include <SG/Entity/NodeGroup.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/ScopedTimer.hpp>

#include <iomanip>
#include <iostream>

#include <gtest/gtest.h>

using namespace TabGraph;

constexpr auto NodeGroupNbr = 10;
constexpr auto NodeNbr = 10;

auto CreateTestScene(std::shared_ptr<ECS::DefaultRegistry>& a_Registry)
{
    SG::Scene scene(a_Registry);
    for (int i = 0; i < NodeGroupNbr; ++i) {
        auto node = SG::NodeGroup::Create(a_Registry);
        node.template GetComponent<SG::Component::Name>() = "node_" + std::to_string(i);
        scene.AddEntity(node);
        for (int j = 0; j < NodeNbr; ++j) {
            auto leaf = SG::Node::Create(a_Registry);
            leaf.template GetComponent<SG::Component::Name>() = "leaf_" + std::to_string((i * 2) + j);
            SG::Node::SetParent(leaf, node);
        }
    }
    return scene;
}

TEST(SG, TestScene)
{
    auto registry = ECS::DefaultRegistry::Create();
    ASSERT_NE(registry, nullptr);
    auto scene    = CreateTestScene(registry);
    ASSERT_EQ(scene.GetByteSize(), sizeof(SG::Scene));
}

TEST(SG, SearchByName)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto scene = CreateTestScene(registry);
    Tools::ScopedTimer timer("Search");
    std::cout << "Search node group by name : \n";
    unsigned count = 0;
    auto view = registry->GetView<NODEGROUP_COMPONENTS>();
    view.ForEach<SG::Component::Name>(
        [&count](auto entity, auto& name) {
            if (std::string(name) == std::string("node_4")) {
                std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << "\n";
                ++count;
            }
        }
    );
    ASSERT_EQ(count, 1);
}

TEST(SG, SearchByTypeNodeGroup)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto scene = CreateTestScene(registry);
    Tools::ScopedTimer timer("Search");
    std::cout << "Search nodes by type : \n";
    unsigned count = 0;
    auto view = registry->GetView<NODEGROUP_COMPONENTS>();
    view.ForEach<SG::Component::Name>(
        [&count](auto entity, auto& name) {
            std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << "\n";
            ++count;
        }
    );
    ASSERT_EQ(count, NodeGroupNbr);
}

TEST(SG, SearchByTypeNode)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto scene = CreateTestScene(registry);
    Tools::ScopedTimer timer("Search");
    std::cout << "Search leaves by type : \n";
    unsigned count = 0;
    auto view = registry->GetView<NODE_COMPONENTS>(ECS::Exclude<SG::Component::Children>{});
    view.ForEach<SG::Component::Name>(
        [&count](auto entity, auto& name) {
            std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << "\n";
            ++count;
        }
    );
    ASSERT_EQ(count, NodeGroupNbr * NodeNbr);
}

TEST(SG, RangeSearchByNameNodeGroup)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto scene = CreateTestScene(registry);
    Tools::ScopedTimer timer("Search");
    std::cout << "Search node group by name : \n";
    unsigned count = 0;
    auto view = registry->GetView<NODEGROUP_COMPONENTS>();
    for (auto& it : view) {
        auto& entity = std::get<0>(it);
        auto& name = std::get<SG::Component::Name&>(it);
        if (std::string(name) == std::string("node_4")) {
            std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << std::endl;
            ++count;
            break;
        }
    }
    ASSERT_EQ(count, 1);
}

TEST(SG, RangeSearchByTypeNodeGroup)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto scene = CreateTestScene(registry);
    Tools::ScopedTimer timer("Search");
    std::cout << "Search nodes by type : \n";
    unsigned count = 0;
    auto view = registry->GetView<NODEGROUP_COMPONENTS>();
    for (auto& it : view) {
        auto& entity = std::get<0>(it);
        auto& name = std::get<SG::Component::Name&>(it);
        ++count;
        std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << std::endl;
    }
    ASSERT_EQ(count, NodeGroupNbr);
}

TEST(SG, RangeSearchByTypeNode)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto scene = CreateTestScene(registry);
    Tools::ScopedTimer timer("Search");
    std::cout << "Search leaves by type : \n";
    unsigned count = 0;
    auto view = registry->GetView<NODE_COMPONENTS>(ECS::Exclude<SG::Component::Children> {});
    for (auto&& [entity, name, transform, parent] : view) {
        std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << std::endl;
        ++count;
    }
    ASSERT_EQ(count, NodeGroupNbr * NodeNbr);
}

TEST(SG, RangeSearchByTypeAllNodes)
{
    auto registry = ECS::DefaultRegistry::Create();
    auto scene = CreateTestScene(registry);
    Tools::ScopedTimer timer("Search");
    std::cout << "Search leaves by type : \n";
    unsigned count = 0;
    auto view = registry->GetView<NODE_COMPONENTS>();
    for (auto&& [entity, name, transform, parent] : view) {
        std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << std::endl;
        ++count;
    }
    //without excluding entities with children we should get ALL nodes
    ASSERT_EQ(count, NodeGroupNbr * NodeNbr + NodeGroupNbr);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
