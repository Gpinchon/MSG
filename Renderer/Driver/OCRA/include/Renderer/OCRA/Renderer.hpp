#pragma once

#include <OCRA/OCRA.hpp>

#include <map>

namespace TabGraph::SG {
class Primitive;
class Material;
}

namespace TabGraph::Renderer {
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

inline auto GetQueue(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device)
{
    const auto queueFamily = OCRA::PhysicalDevice::FindQueueFamily(a_PhysicalDevice, OCRA::QueueFlagBits::Graphics);
    return OCRA::Device::GetQueue(a_Device, queueFamily, 0); //Get first available queue
}
struct Primitive;
struct Material;
struct Impl {
    Impl(const OCRA::ApplicationInfo& a_Info)
        : instance(OCRA::CreateInstance({ a_Info }))
    {
        OCRA::CreateCommandPoolInfo poolInfo;
        poolInfo.flags = OCRA::CreateCommandPoolFlagBits::Reset;
        poolInfo.queueFamilyIndex = OCRA::PhysicalDevice::FindQueueFamily(physicalDevice, OCRA::QueueFlagBits::Graphics);
        commandPool   = OCRA::Device::CreateCommandPool(logicalDevice, poolInfo);
        OCRA::AllocateCommandBufferInfo commandBufferInfo;
        commandBufferInfo.count = 1;
        commandBufferInfo.level = OCRA::CommandBufferLevel::Primary;
        commandBuffer = OCRA::Command::Pool::AllocateCommandBuffer(commandPool, commandBufferInfo).front();
    }
    OCRA::Instance::Handle instance;
    OCRA::PhysicalDevice::Handle physicalDevice{ OCRA::Instance::EnumeratePhysicalDevices(instance).front() };
    OCRA::Device::Handle logicalDevice{ CreateDevice(physicalDevice) };
    OCRA::Command::Pool::Handle   commandPool;
    OCRA::Command::Buffer::Handle commandBuffer;
    OCRA::Queue::Handle queue{ GetQueue(physicalDevice, logicalDevice) };
    std::map<SG::Primitive*, std::shared_ptr<Primitive>> primitives;
    std::map<SG::Material*,  std::shared_ptr<Material>>  materials;
};
}