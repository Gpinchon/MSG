#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Msg::Renderer {
struct CreateRendererInfo;
}

namespace Msg::Renderer {
vk::raii::Instance CreateInstance(const CreateRendererInfo& a_Info);
}