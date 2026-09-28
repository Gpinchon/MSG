#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Renderer/VK/RenderBuffer.hpp>
#include <MSG/Renderer/VK/Renderer.hpp>
#include <cassert>

namespace Msg::RenderBuffer {
static constexpr vk::Format s_RBFormat = vk::Format::eR8G8B8A8Unorm;
Handle Create(
    const Renderer::Handle& a_Renderer,
    const CreateRenderBufferInfo& a_Info)
{
    return std::make_shared<Impl>(
        a_Renderer->device, a_Renderer->physicalDevice,
        vk::ImageType::e2D,
        vk::Extent3D { a_Info.width, a_Info.height, 1 }, s_RBFormat,
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc);
}

std::any GetNativeHandle(const Handle& a_RenderBuffer)
{
    return **a_RenderBuffer;
}

void UploadImage(
    const Handle& a_TargetRenderBuffer,
    const Image& a_SourceImage)
{
    assert(false && "TODO : implement");
}

void DownloadImage(
    const Handle& a_SourceRenderBuffer,
    const Image& a_TargetImage)
{
    assert(false && "TODO : implement");
}
}
