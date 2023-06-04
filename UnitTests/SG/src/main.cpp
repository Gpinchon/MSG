#include <SG/Entity/Node.hpp>
#include <SG/Entity/NodeGroup.hpp>
#include <SG/Scene/Scene.hpp>

#include <Tools/ScopedTimer.hpp>

#include <iomanip>
#include <iostream>

using namespace TabGraph;

int main(int argc, char const* argv[])
{
    // build a test scene
    auto registry = ECS::DefaultRegistry::Create();
    SG::Scene scene(registry);
    for (int i = 0; i < 5; ++i) {
        auto node                                = SG::NodeGroup::Create(registry);
        node.GetComponent<SG::Component::Name>() = "node_" + std::to_string(i);
        scene.AddEntity(node);
        for (int j = 0; j < 2; ++j) {
            auto leaf                                = SG::Node::Create(registry);
            leaf.GetComponent<SG::Component::Name>() = "leaf_" + std::to_string((i * 2) + j);
            SG::Node::SetParent(leaf, node);
        }
    }

    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "ForEach iteration : \n";
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("Search");
        std::cout << "Search node group by name : \n";
        registry->GetView<NODEGROUP_COMPONENTS>().ForEach<SG::Component::Name>([](auto entity, auto& name) {
            if (std::string(name) == std::string("node_4"))
                std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << "\n";
        });
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("Search");
        std::cout << "Search nodes by type : \n";
        registry->GetView<NODEGROUP_COMPONENTS>().ForEach<SG::Component::Name>([](auto entity, auto& name) {
            std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << "\n";
        });
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("Search");
        std::cout << "Search leaves by type : \n";
        registry->GetView<NODE_COMPONENTS>(ECS::Exclude<SG::Component::Children> {}).ForEach<SG::Component::Name>([](auto entity, auto& name) {
            std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << "\n";
        });
    }
    std::cout << "--------------------------------------------------------------------------------\n";

    std::cout << "--------------------------------------------------------------------------------\n";
    std::cout << "Range based iteration : \n";
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("Search");
        std::cout << "Search node group by name : \n";
        for (auto& it : registry->GetView<NODEGROUP_COMPONENTS>()) {
            auto& entity = std::get<0>(it);
            auto& name   = std::get<SG::Component::Name&>(it);
            if (std::string(name) == std::string("node_4")) {
                std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << std::endl;
                break;
            }
        }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("Search");
        std::cout << "Search nodes by type : \n";
        for (auto& it : registry->GetView<NODEGROUP_COMPONENTS>()) {
            auto& entity = std::get<0>(it);
            auto& name   = std::get<SG::Component::Name&>(it);
            std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << std::endl;
        }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    {
        Tools::ScopedTimer timer("Search");
        std::cout << "Search leaves by type : \n";
        for (auto&& [entity, name, transform, parent] : registry->GetView<NODE_COMPONENTS>(ECS::Exclude<SG::Component::Children> {})) {
            std::cout << "Entity " << std::setw(2) << entity << " : " << std::string(name) << std::endl;
        }
    }
    std::cout << "--------------------------------------------------------------------------------\n";
    return 0;
}
