#pragma once

#include <vulkan/vulkan_raii.hpp>

#include <string>

namespace Msg {
class VKInstance {
public:
    VKInstance(const std::string& a_AppName, const uint32_t& a_AppVersion);
    std::vector<vk::LayerProperties> GetAvailableLayers() const;
    operator auto&() { return instance; }
    operator auto&() const { return instance; }
    // context should be destroyed last, no dedicated class to avoid useless complexity
    vk::raii::Context context = { };
    vk::raii::Instance instance;
};
}
