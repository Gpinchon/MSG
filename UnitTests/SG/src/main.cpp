#include <SG/Node/Node.hpp>
#include <SG/Node/NodeGroup.hpp>
#include <SG/Node/Scene.hpp>
#include <SG/Visitor/SearchVisitor.hpp>

#include <iostream>

using namespace TabGraph;

int main(int argc, char const *argv[])
{
    //build a test scene
    auto scene{ std::make_shared<SG::Scene>() };
    auto node0{ std::make_shared<SG::NodeGroup>("node0") };
    for (int i = 0; i < 5; ++i) {
        auto testNode{ std::make_shared<SG::NodeGroup>("node1") };
        testNode->SetParent(node0);
        for (int j = 0; j < 2; ++j) {
            auto testNode1{ std::make_shared<SG::Node>("node2") };
            testNode1->SetParent(testNode);
        }
    }
    scene->AddNode(node0);

    //test search visitor
    {
        SG::SearchVisitor search(std::string("node2"), SG::NodeVisitor::Mode::VisitChildren);
        scene->Accept(search);
        std::cout << "Search by name : \n";
        for (const auto& obj : search.GetResult())
            std::cout << obj << " : " << obj->GetName() << "\n";
    }

    //test search by type
    {
        SG::SearchVisitor search(typeid(SG::Node), SG::NodeVisitor::Mode::VisitChildren);
        scene->Accept(search);
        std::cout << "Search by type : \n";
        for (const auto& obj : search.GetResult())
            std::cout << obj << " : " << obj->GetName() << "\n";
    }
    return 0;
}