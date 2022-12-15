#pragma once

#include <OCRA/Instance.hpp>
#include <OCRA/PhysicalDevice.hpp>
#include <OCRA/Device.hpp>

namespace TabGraph::Renderer {
std::vector<OCRA::Queue::Info> GetQueueInfos(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice)
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

OCRA::Device::Handle CreateDevice(const OCRA::PhysicalDevice::Handle& a_PhysicalDevice)
{
    OCRA::Device::Info deviceInfo;
    deviceInfo.queueInfos = GetQueueInfos(a_PhysicalDevice);
    return OCRA::Device::Create(a_PhysicalDevice, deviceInfo);
}

struct Impl {
    Impl(const OCRA::Application::Info& a_Info)
        : _instance(OCRA::Instance::Create({ a_Info }))
    {}
    OCRA::Instance::Handle _instance;
    OCRA::PhysicalDevice::Handle _physicalDevice{ OCRA::Instance::EnumeratePhysicalDevices(_instance).front() };
    OCRA::Device::Handle _logicalDevice{ CreateDevice(_physicalDevice) };
};
}