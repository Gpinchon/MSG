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

inline std::vector<OCRA::QueueInfo> GetQueueInfos(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice)
{
    std::vector<OCRA::QueueInfo> queueInfos;
    auto& queueFamilies = OCRA::PhysicalDevice::GetQueueFamilyProperties(a_PhysicalDevice);
    uint32_t familyIndex = 0;
    for (auto& queueFamily : queueFamilies)
    {
        OCRA::QueueInfo queueInfo;
        queueInfo.queueCount = queueFamily.queueCount;
        queueInfo.queueFamilyIndex = familyIndex;
        queueInfo.queuePriorities.resize(queueFamily.queueCount, 1.f);
        queueInfos.push_back(queueInfo);
        ++familyIndex;
    }
    return queueInfos;
}

inline OCRA::Device::Handle CreateDevice(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice)
{
    OCRA::CreateDeviceInfo deviceInfo;
    deviceInfo.queueInfos = GetQueueInfos(a_PhysicalDevice);
    return OCRA::PhysicalDevice::CreateDevice(a_PhysicalDevice, deviceInfo);
}

inline OCRA::Queue::Handle GetQueue(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device)
{
    const auto queueFamily = OCRA::PhysicalDevice::FindQueueFamily(a_PhysicalDevice, OCRA::QueueFlagBits::Graphics);
    return OCRA::Device::GetQueue(a_Device, queueFamily, 0); //Get first available queue
}

void Load(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene)
{
    a_Renderer->Load(a_Scene);
}

void Render(
    const Handle& a_Renderer,
    const SG::Scene& a_Scene,
    const RenderBuffer::Handle& a_Buffer)
{
    a_Renderer->Render(a_Scene, a_Buffer);
}

void Update(const Handle& a_Renderer)
{
    a_Renderer->Update();
}

Impl::Impl(const OCRA::ApplicationInfo& a_Info)
    : instance(OCRA::CreateInstance({ a_Info }))
    , physicalDevice(OCRA::Instance::EnumeratePhysicalDevices(instance).front())
    , logicalDevice(CreateDevice(physicalDevice))
    , queue(GetQueue(physicalDevice, logicalDevice))
{
    OCRA::CreateCommandPoolInfo poolInfo;
    poolInfo.flags = OCRA::CreateCommandPoolFlagBits::Reset;
    poolInfo.queueFamilyIndex = OCRA::PhysicalDevice::FindQueueFamily(physicalDevice, OCRA::QueueFlagBits::Graphics);
    commandPool = OCRA::Device::CreateCommandPool(logicalDevice, poolInfo);
    OCRA::AllocateCommandBufferInfo commandBufferInfo;
    commandBufferInfo.count = 1;
    commandBufferInfo.level = OCRA::CommandBufferLevel::Primary;
    commandBuffer = OCRA::Command::Pool::AllocateCommandBuffer(commandPool, commandBufferInfo).front();
}

Impl::~Impl()
{
    OCRA::Queue::WaitIdle(queue);
    OCRA::Command::Buffer::Reset(commandBuffer);
}

void Impl::Load(const SG::Scene& a_Scene)
{
    auto& registry = a_Scene.GetRegistry();
    auto view = registry->GetView<SG::Component::Mesh>(ECS::Exclude<Component::MeshData>{});
    view.ForEach<SG::Component::Mesh>([renderer = this, registry](auto entityID, const auto& mesh) {
        registry->AddComponent<Component::MeshData>(entityID, renderer, mesh);
    });
}

void Impl::Render(const SG::Scene& a_Scene, const RenderBuffer::Handle& a_Buffer)
{
    auto& registry = a_Scene.GetRegistry();
    auto view = registry->GetView<Component::MeshData>();
    OCRA::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = OCRA::CommandBufferUsageFlagBits::OneTimeSubmit;
    OCRA::Command::Buffer::Begin(commandBuffer, beginInfo);
    {
        OCRA::ImageLayoutTransitionInfo renderTargetTransition;
        renderTargetTransition.image = a_Buffer->image;
        renderTargetTransition.oldLayout = OCRA::ImageLayout::Undefined;
        renderTargetTransition.newLayout = OCRA::ImageLayout::ColorAttachmentOptimal;
        renderTargetTransition.subRange.aspects = OCRA::ImageAspectFlagBits::Color;
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
                [this, a_Buffer](const auto& meshData) {

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
void Impl::Update()
{
    {
        std::vector<decltype(primitives)::key_type> toDelete;
        for (auto& primitive : primitives) {
            if (primitive.second.use_count() == 1) //nobody else uses that
                toDelete.push_back(primitive.first);
        }
        for (const auto& primitive : toDelete)
            primitives.erase(primitive);
    }
    {
        std::vector<decltype(materials)::key_type> toDelete;
        for (auto& primitive : materials) {
            if (primitive.second.use_count() == 1) //nobody else uses that
                toDelete.push_back(primitive.first);
        }
        for (const auto& primitive : toDelete)
            materials.erase(primitive);
    }
}
}
