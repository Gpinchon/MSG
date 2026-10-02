#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Msg::Renderer {
struct CreateRendererInfo;
struct RendererSettings;
}

namespace Msg::Renderer {
class Impl {
public:
    Impl(const CreateRendererInfo& a_Info);
    ~Impl();
    void Update();
    void SetSettings(const RendererSettings& a_Settings);
    vk::raii::Context context;
    vk::raii::Instance instance;
    vk::raii::PhysicalDevice physicalDevice;
    vk::raii::Device device;
    vk::raii::CommandPool cmdPool;
    vk::raii::Queue transferQueue;
    vk::raii::Queue graphicsQueue;
    vk::raii::Queue computeQueue;
    vk::raii::Queue presentQueue;
    // vk::raii::CommandBuffer transferCmdBuffer;
    vk::raii::Semaphore transferSemaphore;
    vk::raii::Semaphore graphicsSemaphore;
    vk::raii::Semaphore computeSemaphore;
};
}
