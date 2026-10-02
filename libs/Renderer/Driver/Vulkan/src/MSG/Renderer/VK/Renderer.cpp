#include <MSG/Debug.hpp>
#include <MSG/Renderer/Structs.hpp>
#include <MSG/Renderer/VK/Device.hpp>
#include <MSG/Renderer/VK/Instance.hpp>
#include <MSG/Renderer/VK/PhysicalDevice.hpp>
#include <MSG/Renderer/VK/Renderer.hpp>

namespace Msg::Renderer {
static std::array<glm::uvec3, 4> s_defaultVolumetricFogResolution {
    glm::uvec3(32, 32, 16),
    glm::uvec3(64, 64, 32),
    glm::uvec3(96, 96, 64),
    glm::uvec3(128, 128, 128),
};

glm::uvec3 GetDefaultVolumetricFogRes(const QualitySetting& a_Quality)
{
    return s_defaultVolumetricFogResolution.at(int(a_Quality));
}

auto GetQueue(
    const vk::raii::Device& a_Device,
    const uint32_t& a_FamilyIndex,
    const uint32_t& a_QueueIndex = 0)
{
    return a_Device.getQueue(a_FamilyIndex, a_QueueIndex);
}

static vk::raii::CommandPool CreateCommandPool(
    const vk::raii::Device& a_device,
    const uint32_t& a_queueFamilyIndex,
    const vk::CommandPoolCreateFlags& a_flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer)
{
    vk::CommandPoolCreateInfo poolInfo { };
    poolInfo.flags            = a_flags;
    poolInfo.queueFamilyIndex = a_queueFamilyIndex;
    return a_device.createCommandPool(poolInfo);
}

Impl::Impl(const CreateRendererInfo& a_Info)
    : instance(CreateInstance(context, a_Info))
    , physicalDevice(vk::raii::PhysicalDevices(instance).front())
    , device(CreateDevice(physicalDevice))
    , cmdPool(CreateCommandPool(device, FindQueueFamily(physicalDevice, s_QueueFlags)))
    , transferQueue(GetQueue(device, FindQueueFamily(physicalDevice, s_QueueFlags), 0))
    , graphicsQueue(GetQueue(device, FindQueueFamily(physicalDevice, s_QueueFlags), 1))
    , computeQueue(GetQueue(device, FindQueueFamily(physicalDevice, s_QueueFlags), 2))
    , presentQueue(GetQueue(device, FindQueueFamily(physicalDevice, s_QueueFlags), 3))
    , transferSemaphore(device.createSemaphore(vk::SemaphoreCreateInfo { }))
    , graphicsSemaphore(device.createSemaphore(vk::SemaphoreCreateInfo { }))
    , computeSemaphore(device.createSemaphore(vk::SemaphoreCreateInfo { }))
{
}

Impl::~Impl()
{
    MSGDebugLog("Renderer instance destroyed.");
}

void Impl::Update()
{
}

void Impl::SetSettings(const RendererSettings& a_Settings)
{
}

Handle Create(const CreateRendererInfo& a_Info, const RendererSettings& a_Settings)
{
    auto renderer = std::make_shared<Impl>(a_Info);
    renderer->SetSettings(a_Settings);
    return renderer;
}

void Update(const Handle& a_Renderer)
{
    a_Renderer->Update();
}
}
