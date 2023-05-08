#include <Renderer/Renderer.hpp>
#include <Renderer/OCRA/Renderer.hpp>
#include <Renderer/OCRA/RenderBuffer.hpp>
#include <Renderer/OCRA/Component/MeshData.hpp>

#include <SG/Scene/Scene.hpp>
#include <SG/Component/Mesh.hpp>

#include <OCRA/OCRA.hpp>

namespace TabGraph::Renderer {
Handle Create(const CreateRendererInfo& a_Info) {
    OCRA::ApplicationInfo info;
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
    auto& commandBuffer = a_Renderer->commandBuffer;
    auto& queue = a_Renderer->queue;
    OCRA::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = OCRA::CommandBufferUsageFlagBits::OneTimeSubmit;
    OCRA::Command::Buffer::Begin(commandBuffer, beginInfo);
    {
        OCRA::ImageLayoutTransitionInfo renderTargetTransition;
        renderTargetTransition.image = a_Buffer->image;
        renderTargetTransition.oldLayout = OCRA::ImageLayout::Undefined;
        renderTargetTransition.newLayout = OCRA::ImageLayout::ColorAttachmentOptimal;
        OCRA::Command::TransitionImageLayout(
            commandBuffer, renderTargetTransition
        );
        OCRA::RenderingInfo renderingInfo;
        OCRA::RenderingAttachmentInfo colorAttachment;
        colorAttachment.imageView = a_Buffer->imageView;
        colorAttachment.imageLayout = OCRA::ImageLayout::General;
        colorAttachment.storeOp = OCRA::StoreOp::Store;
        renderingInfo.colorAttachments.push_back(colorAttachment);
        renderingInfo.layerCount = 1;
        OCRA::Command::BeginRendering(commandBuffer, renderingInfo);
        {
            view.ForEach<Component::MeshData>(
                [a_Renderer, a_Buffer](const auto& meshData) {

                }
            );
        }
        OCRA::Command::EndRendering(commandBuffer);
    }
    OCRA::Command::Buffer::End(commandBuffer);
    OCRA::QueueSubmitInfo submitInfo;
    submitInfo.commandBuffers = { commandBuffer };
    OCRA::Queue::Submit(queue, { submitInfo });
}
void Update(const Handle& a_Renderer)
{
    {
        std::vector<decltype(a_Renderer->primitives)::key_type> toDelete;
        for (auto& primitive : a_Renderer->primitives) {
            if (primitive.second.use_count() == 1) //nobody else uses that
                toDelete.push_back(primitive.first);
        }
        for (const auto& primitive : toDelete)
            a_Renderer->primitives.erase(primitive);
    }
    {
        std::vector<decltype(a_Renderer->materials)::key_type> toDelete;
        for (auto& primitive : a_Renderer->materials) {
            if (primitive.second.use_count() == 1) //nobody else uses that
                toDelete.push_back(primitive.first);
        }
        for (const auto& primitive : toDelete)
            a_Renderer->materials.erase(primitive);
    }
}
}
