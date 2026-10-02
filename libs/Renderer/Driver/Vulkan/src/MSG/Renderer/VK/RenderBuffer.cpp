#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Renderer/VK/RenderBuffer.hpp>
#include <MSG/Renderer/VK/Renderer.hpp>
#include <MSG/VKInvoke.hpp>
#include <MSG/Image.hpp>
#include <MSG/ImageUtils.hpp>
#include <MSG/Debug.hpp>
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
        vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eHostTransfer);
}

std::any GetNativeHandle(const Handle& a_RenderBuffer)
{
    return **a_RenderBuffer;
}

void UploadImage(
    const Renderer::Handle& a_Renderer,
    const Handle& a_DstRenderBuffer,
    const Image& a_SrcImage,
    const ImageCopyInfo& a_CopyInfo)
{
    // Transition renderbuffer to transfer dst optimal
    {
        vk::HostImageLayoutTransitionInfo imageTransitionInfo;
        imageTransitionInfo.image            = *a_DstRenderBuffer;
        imageTransitionInfo.oldLayout        = vk::ImageLayout::eUndefined;
        imageTransitionInfo.newLayout        = vk::ImageLayout::eTransferDstOptimal;
        imageTransitionInfo.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        a_Renderer->device.transitionImageLayout(imageTransitionInfo);
    }
    glm::uvec3 copySize;
    if (a_CopyInfo.srcExtent.x == 0xFFFFFFFF)
        copySize.x = a_SrcImage.GetSize().x - a_CopyInfo.srcOffset.x;
    if (a_CopyInfo.srcExtent.y == 0xFFFFFFFF)
        copySize.y = a_SrcImage.GetSize().y - a_CopyInfo.srcOffset.y;
    if (a_CopyInfo.srcExtent.z == 0xFFFFFFFF)
        copySize.z = a_SrcImage.GetSize().z - a_CopyInfo.srcOffset.z;
    MSGCheckErrorFatal(glm::any(glm::greaterThan(a_CopyInfo.srcOffset + copySize, a_SrcImage.GetSize())), "Image copy out of range !");
    auto imageData = ImageConvert(a_SrcImage, PixelSizedFormat::Uint8_NormalizedRGBA).Read(a_CopyInfo.srcOffset, copySize);
    vk::MemoryToImageCopy copy;
    copy.setPHostPointer(imageData.data());
    copy.setMemoryRowLength(0); // data tightly packed
    copy.setMemoryImageHeight(0);
    copy.setImageOffset(vk::Offset3D(a_CopyInfo.dstOffset.x, a_CopyInfo.dstOffset.y, a_CopyInfo.dstOffset.z));
    copy.setImageExtent({ copySize.x, copySize.y, copySize.z });
    copy.imageSubresource.setAspectMask(vk::ImageAspectFlagBits::eColor);
    copy.imageSubresource.setBaseArrayLayer(0);
    copy.imageSubresource.setLayerCount(1);
    copy.imageSubresource.setMipLevel(0);
    vk::CopyMemoryToImageInfo copyInfo;
    copyInfo.setDstImage(*a_DstRenderBuffer);
    copyInfo.setDstImageLayout(vk::ImageLayout::eTransferDstOptimal);
    copyInfo.setRegions(copy);
    a_Renderer->device.copyMemoryToImage(copyInfo);
    // Transition renderbuffer back to general
    {
        vk::HostImageLayoutTransitionInfo imageTransitionInfo;
        imageTransitionInfo.image            = *a_DstRenderBuffer;
        imageTransitionInfo.oldLayout        = vk::ImageLayout::eTransferDstOptimal;
        imageTransitionInfo.newLayout        = vk::ImageLayout::eGeneral;
        imageTransitionInfo.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        a_Renderer->device.transitionImageLayout(imageTransitionInfo);
    }
}

void DownloadImage(
    const Handle& a_SourceRenderBuffer,
    const Image& a_TargetImage)
{
    assert(false && "TODO : implement");
}
}
