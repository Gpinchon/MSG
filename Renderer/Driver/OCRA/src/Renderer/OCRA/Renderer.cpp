#include <Renderer/Renderer.hpp>
#include <Renderer/OCRA/Renderer.hpp>
#include <Renderer/OCRA/Component/MeshData.hpp>

#include <SG/Scene/Scene.hpp>
#include <SG/Component/Mesh.hpp>

#include <OCRA/Instance.hpp>

namespace TabGraph::Renderer {
Handle Create(const Info& a_Info) {
    OCRA::Application::Info info;
    info.name = a_Info.name;
    info.applicationVersion = info.applicationVersion;
    info.engineVersion = 100;
    info.engineName = "TabGraph";
    return Handle(new Impl(info));
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    auto& registry = a_Scene.GetRegistry();
    auto view = registry->GetView<SG::Component::Mesh>(ECS::Exclude<Component::MeshData>{});
    view.ForEach<SG::Component::Mesh>([=](auto entityID, const auto& mesh) {
        registry->AddComponent<Component::MeshData>(entityID, a_Renderer, mesh);
    });
}

void Render(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene,
    const RenderBuffer::Handle& a_Buffer)
{
    auto& registry = a_Scene.GetRegistry();
    auto view = registry->GetView<Component::MeshData>();
    view.ForEach<Component::MeshData>([](const auto& meshData) {

    });
}
}
