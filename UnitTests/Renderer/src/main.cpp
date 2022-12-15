#include <ECS/Registry.hpp>

#include <SG/Entity/Node/Node.hpp>
#include <SG/Scene/Scene.hpp>
#include <SG/ShapeGenerator/Cube.hpp>

#include <Renderer/Renderer.hpp>
#include <Renderer/Window.hpp>

#include <Tools/ScopedTimer.hpp>

using namespace TabGraph;

int main(int argc, char const *argv[])
{
    auto registry = ECS::DefaultRegistry::Create();
    auto renderer = Renderer::Create({ "UnitTest", 100 });
    SG::Scene testScene(registry, "testScene");

    //build a test scene
    {
        auto testEntity = SG::Node::Create(registry);
        testEntity.AddComponent<SG::Component::Mesh>(SG::Cube::CreateMesh("testCube", { 1, 1, 1 }));
        testScene.AddEntity(testEntity);
    }
    int v;
    std::cin >> v;
    return 0;
}