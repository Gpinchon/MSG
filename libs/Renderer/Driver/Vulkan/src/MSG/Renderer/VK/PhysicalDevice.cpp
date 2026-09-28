#include <MSG/Renderer/VK/PhysicalDevice.hpp>

uint32_t Msg::Renderer::FindQueueFamily(
    const vk::raii::PhysicalDevice& a_PhysicalDevice,
    const vk::QueueFlags& a_QueueFlags)
{
    auto queueProperties = a_PhysicalDevice.getQueueFamilyProperties();
    for (auto familyIndex = 0u; familyIndex < queueProperties.size(); ++familyIndex) {
        // check if a_QueueFlags is a subset of queueFlags
        if ((queueProperties.at(familyIndex).queueFlags & a_QueueFlags) == a_QueueFlags)
            return familyIndex;
    }
    return std::numeric_limits<uint32_t>::infinity();
}

vk::raii::Device Msg::Renderer::CreateDevice(const vk::raii::PhysicalDevice& a_PhysicalDevice)
{
    const std::vector<const char*> extensions {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
        // VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
        // VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
        VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME
    };
    std::vector<vk::DeviceQueueCreateInfo> queues;
    const std::vector<const char*> layers { };
    const std::vector<float> queuePriorities {
        0, 0, 0, 0
    };
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setQueueFamilyIndex(FindQueueFamily(a_PhysicalDevice, s_QueueFlags));
    queueCreateInfo.setQueuePriorities(queuePriorities);
    queues.push_back(queueCreateInfo);

    vk::PhysicalDeviceFeatures enabledFeatures;
    enabledFeatures.depthClamp = true;
    vk::PhysicalDeviceCustomBorderColorFeaturesEXT customBorder(true, true);
    vk::PhysicalDeviceDynamicRenderingFeaturesKHR dynamicRenderingFeature(true, &customBorder);
    vk::PhysicalDeviceVertexInputDynamicStateFeaturesEXT dynamicVertexInputFeature(true, &dynamicRenderingFeature);
    vk::PhysicalDeviceTimelineSemaphoreFeatures timelineSemaphoreFeature(true, &dynamicVertexInputFeature);
    vk::DeviceCreateInfo info(
        vk::DeviceCreateFlags { },
        queues,
        layers,
        extensions,
        &enabledFeatures,
        &timelineSemaphoreFeature);
    return a_PhysicalDevice.createDevice(info);
}
