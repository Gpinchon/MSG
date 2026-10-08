#include <MSG/VKMemoryAllocator.hpp>
#include <MSG/Debug.hpp>

struct VKMemoryDeleter {
    VKMemoryDeleter(const size_t& a_Size)
        : size(a_Size)
    {
    }
    void operator()(vk::raii::DeviceMemory& a_Memory)
    {
    }
    const size_t size;
};

std::shared_ptr<vk::raii::DeviceMemory> Msg::VKMemoryAllocator::AllocateMemory(const vk::MemoryRequirements& a_MemRequirements, const vk::MemoryPropertyFlags& a_Properties)
{
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i) {
        if ((a_MemRequirements.memoryTypeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & a_Properties) == a_Properties) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        throw std::runtime_error("Failed to find suitable memory type!");
    }

    vk::MemoryAllocateInfo allocInfo { };
    allocInfo.allocationSize  = a_MemRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;
    allocated += a_MemRequirements.size;
    return std::shared_ptr<vk::raii::DeviceMemory>(new vk::raii::DeviceMemory(device, allocInfo), [this, size = a_MemRequirements.size](auto& a_Ptr) {
        MSGCheckErrorFatal(allocated < size, "Allocated memory is less than this device memory size, maybe a double free ?");
        allocated -= size;
        delete a_Ptr;
    });
}