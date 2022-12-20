#pragma once

#include <OCRA/Instance.hpp>
#include <OCRA/PhysicalDevice.hpp>
#include <OCRA/Device.hpp>

#include <map>

namespace TabGraph::SG {
struct Primitive;
}

namespace TabGraph::Renderer {
inline std::vector<OCRA::Queue::Info> GetQueueInfos(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice)
{
    std::vector<OCRA::Queue::Info> queueInfos;
    auto& queueFamilies = OCRA::PhysicalDevice::GetQueueFamilyProperties(a_PhysicalDevice);
    uint32_t familyIndex = 0;
    for (auto& queueFamily : queueFamilies)
    {
        OCRA::Queue::Info queueInfo;
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
    OCRA::Device::Info deviceInfo;
    deviceInfo.queueInfos = GetQueueInfos(a_PhysicalDevice);
    return OCRA::Device::Create(a_PhysicalDevice, deviceInfo);
}

inline uint32_t FindQueueFamily(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::PhysicalDevice::QueueFlags& a_QueueProperties)
{
    auto& queueProperties = OCRA::PhysicalDevice::GetQueueFamilyProperties(a_PhysicalDevice);
    for (auto familyIndex = 0u; familyIndex < queueProperties.size(); ++familyIndex) {
        if (queueProperties.at(familyIndex).queueFlags == a_QueueProperties)
            return familyIndex;
    }
    return std::numeric_limits<uint32_t>::infinity();
}

inline auto GetQueue(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice, const OCRA::Device::Handle& a_Device)
{
    const auto queueFamily = FindQueueFamily(a_PhysicalDevice, OCRA::PhysicalDevice::QueueFlagsBits::Graphics);
    return OCRA::Device::GetQueue(a_Device, queueFamily, 0); //Get first available queue
}
struct Primitive;
struct Impl {
    Impl(const OCRA::Application::Info& a_Info)
        : instance(OCRA::Instance::Create({ a_Info }))
    {}
    OCRA::Instance::Handle instance;
    OCRA::PhysicalDevice::Handle physicalDevice{ OCRA::Instance::EnumeratePhysicalDevices(instance).front() };
    OCRA::Device::Handle logicalDevice{ CreateDevice(physicalDevice) };
    OCRA::Queue::Handle queue{ GetQueue(physicalDevice, logicalDevice) };
    std::map<std::weak_ptr<SG::Primitive>, std::shared_ptr<Primitive>, std::owner_less<>> primitives;
};
}