#pragma once

#include <vulkan/vulkan_raii.hpp>

namespace Msg::Renderer {
struct CreateRendererInfo;
}

namespace Msg::Renderer {
vk::raii::Instance CreateInstance(const vk::raii::Context& a_Context, const CreateRendererInfo& a_Info);
}