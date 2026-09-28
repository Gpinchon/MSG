#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Msg::SwapChain {
struct CreateSwapChainInfo;
}

namespace Msg::SwapChain {
class Surface : public vk::raii::SurfaceKHR {
public:
    Surface(vk::raii::Instance& a_Instance, const CreateSwapChainInfo& a_Info);
};
}
