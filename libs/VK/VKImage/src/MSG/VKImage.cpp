#include <MSG/VKImage.hpp>
#include <MSG/VKMemoryAllocator.hpp>

static vk::raii::ImageView CreateView(
    const vk::raii::Device& a_Device,
    const vk::Image& a_Image,
    const vk::Format& a_Format,
    const vk::ImageAspectFlags& a_AspectMask,
    const uint32_t& a_MipLevels,
    const uint32_t& a_ArrayLayers,
    const vk::ImageViewType& a_ViewType = vk::ImageViewType::e2D)
{
    vk::ImageViewCreateInfo viewInfo { };
    viewInfo.image            = a_Image;
    viewInfo.viewType         = a_ViewType;
    viewInfo.format           = a_Format;
    viewInfo.subresourceRange = vk::ImageSubresourceRange {
        a_AspectMask,
        0, a_MipLevels,
        0, a_ArrayLayers
    };

    return vk::raii::ImageView(a_Device, viewInfo);
}

void Msg::VKImage::TransitionLayout(
    const vk::CommandBuffer& a_commandBuffer,
    const vk::Image& a_image,
    const vk::Format& a_format,
    const vk::ImageLayout& a_oldLayout,
    const vk::ImageLayout& a_newLayout,
    const vk::ImageSubresourceRange& a_SubResource)
{
    vk::ImageMemoryBarrier barrier { };
    barrier.oldLayout           = a_oldLayout;
    barrier.newLayout           = a_newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image               = a_image;

    // Aspect mask
    vk::ImageAspectFlags aspectMask = vk::ImageAspectFlagBits::eColor;
    if (a_newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        aspectMask = vk::ImageAspectFlagBits::eDepth;
        if (a_format == vk::Format::eD32SfloatS8Uint || a_format == vk::Format::eD24UnormS8Uint) {
            aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    }
    barrier.subresourceRange                = a_SubResource;
    vk::PipelineStageFlags sourceStage      = GetLayoutTransitionStageFlags(a_oldLayout);
    vk::PipelineStageFlags destinationStage = GetLayoutTransitionStageFlags(a_newLayout);
    barrier.srcAccessMask                   = GetLayoutTransitionAccessFlags(a_oldLayout);
    barrier.dstAccessMask                   = GetLayoutTransitionAccessFlags(a_newLayout);
    a_commandBuffer.pipelineBarrier(
        sourceStage, destinationStage,
        vk::DependencyFlags { },
        nullptr, // memory barriers
        nullptr, // buffer memory barriers
        barrier // image memory barriers
    );
}

static vk::ImageCreateInfo MakeImageCreateInfo(
    const vk::Extent3D& a_Extent,
    const vk::Format& a_Format,
    const vk::ImageUsageFlags& a_Usage,
    const vk::ImageTiling& a_Tiling,
    const uint32_t& a_MipLevels,
    const uint32_t& a_ArrayLayers,
    const vk::SampleCountFlagBits& a_Samples,
    const vk::ImageType& a_ImageType)
{
    vk::ImageCreateInfo imageInfo { };
    imageInfo.imageType     = a_ImageType;
    imageInfo.extent        = a_Extent;
    imageInfo.mipLevels     = a_MipLevels;
    imageInfo.arrayLayers   = a_ArrayLayers;
    imageInfo.format        = a_Format;
    imageInfo.tiling        = a_Tiling;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;
    imageInfo.usage         = a_Usage;
    imageInfo.sharingMode   = vk::SharingMode::eExclusive;
    imageInfo.samples       = a_Samples;
    return imageInfo;
}

Msg::VKImage::VKImage(
    const vk::raii::Device& a_Device,
    const vk::raii::PhysicalDevice& a_PhysicalDevice,
    const vk::ImageType& a_ImageType,
    const vk::Extent3D& a_Extent,
    const vk::Format& a_Format,
    const vk::ImageUsageFlags& a_Usage,
    VKMemoryAllocator& a_MemoryAllocator,
    const vk::MemoryPropertyFlags& a_MemoryProperties,
    const vk::ImageAspectFlags& a_AspectMask,
    const vk::ImageTiling& a_Tiling,
    const uint32_t& a_MipLevels,
    const uint32_t& a_ArrayLayers,
    const vk::SampleCountFlagBits& a_Samples)
    : extent(a_Extent)
    , format(a_Format)
    , mipLevels(a_MipLevels)
    , arrayLayers(a_ArrayLayers)
    , image(a_Device, MakeImageCreateInfo(a_Extent, a_Format, a_Usage, a_Tiling, a_MipLevels, a_ArrayLayers, a_Samples, a_ImageType))
    , memory(a_MemoryAllocator.AllocateMemory(image.getMemoryRequirements(), a_MemoryProperties))
{
    image.bindMemory(*memory, 0);
}

void Msg::VKImage::TransitionLayout(const vk::CommandBuffer& a_CmdBuffer, const vk::ImageLayout& a_OldLayout, const vk::ImageLayout& a_NewLayout, const vk::ImageSubresourceRange& a_SubResource) const
{
    return TransitionLayout(a_CmdBuffer, *image, format, a_OldLayout, a_NewLayout, a_SubResource);
}

vk::PipelineStageFlags Msg::VKImage::GetLayoutTransitionStageFlags(const vk::ImageLayout& a_Layout)
{
    switch (a_Layout) {
    case vk::ImageLayout::eUndefined:
        return vk::PipelineStageFlagBits::eTopOfPipe;
    case vk::ImageLayout::eGeneral:
        return vk::PipelineStageFlagBits::eAllCommands | vk::PipelineStageFlagBits::eHost;
    case vk::ImageLayout::eColorAttachmentOptimal:
        return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        return vk::PipelineStageFlagBits::eFragmentShader;
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
        break;
    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return vk::PipelineStageFlagBits::eFragmentShader
            | vk::PipelineStageFlagBits::eVertexShader
            | vk::PipelineStageFlagBits::eTessellationControlShader
            | vk::PipelineStageFlagBits::eTessellationEvaluationShader;
    case vk::ImageLayout::eTransferSrcOptimal:
    case vk::ImageLayout::eTransferDstOptimal:
        return vk::PipelineStageFlagBits::eTransfer;
    case vk::ImageLayout::ePreinitialized:
        return vk::PipelineStageFlagBits::eHost;
    case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
        break;
    case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
        break;
    case vk::ImageLayout::eDepthAttachmentOptimal:
        break;
    case vk::ImageLayout::eDepthReadOnlyOptimal:
        break;
    case vk::ImageLayout::eStencilAttachmentOptimal:
        break;
    case vk::ImageLayout::eStencilReadOnlyOptimal:
        break;
    case vk::ImageLayout::eReadOnlyOptimal:
        break;
    case vk::ImageLayout::eAttachmentOptimal:
        return vk::PipelineStageFlagBits::eColorAttachmentOutput;
    case vk::ImageLayout::ePresentSrcKHR:
        return vk::PipelineStageFlagBits::eBottomOfPipe;
    case vk::ImageLayout::eVideoDecodeDstKHR:
        break;
    case vk::ImageLayout::eVideoDecodeSrcKHR:
        break;
    case vk::ImageLayout::eVideoDecodeDpbKHR:
        break;
    case vk::ImageLayout::eSharedPresentKHR:
        return vk::PipelineStageFlagBits::eBottomOfPipe;
    case vk::ImageLayout::eFragmentDensityMapOptimalEXT:
        break;
    case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
        break;
    case vk::ImageLayout::eAttachmentFeedbackLoopOptimalEXT:
        break;
    default:
        throw std::runtime_error("Unknown Image Layout !");
    }
    throw std::runtime_error("Error: unsupported layout transition!");
    return vk::PipelineStageFlagBits(-1);
}

vk::AccessFlags Msg::VKImage::GetLayoutTransitionAccessFlags(const vk::ImageLayout& a_Layout)
{
    switch (a_Layout) {
    case vk::ImageLayout::eUndefined:
    case vk::ImageLayout::ePresentSrcKHR:
        return vk::AccessFlagBits::eNone;
    case vk::ImageLayout::eGeneral:
        return vk::AccessFlagBits::eColorAttachmentWrite
            | vk::AccessFlagBits::eDepthStencilAttachmentWrite
            | vk::AccessFlagBits::eTransferWrite
            | vk::AccessFlagBits::eTransferRead
            | vk::AccessFlagBits::eShaderRead
            | vk::AccessFlagBits::eHostWrite
            | vk::AccessFlagBits::eHostRead
            | vk::AccessFlagBits::eInputAttachmentRead
            | vk::AccessFlagBits::eColorAttachmentRead
            | vk::AccessFlagBits::eDepthStencilAttachmentRead
            | vk::AccessFlagBits::eMemoryRead
            | vk::AccessFlagBits::eMemoryWrite;
    case vk::ImageLayout::eColorAttachmentOptimal:
        return vk::AccessFlagBits::eColorAttachmentRead
            | vk::AccessFlagBits::eColorAttachmentWrite;

    case vk::ImageLayout::eStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilAttachmentOptimal:
        return vk::AccessFlagBits::eDepthStencilAttachmentRead
            | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
    case vk::ImageLayout::eDepthAttachmentStencilReadOnlyOptimal:
    case vk::ImageLayout::eDepthReadOnlyOptimal:
    case vk::ImageLayout::eDepthReadOnlyStencilAttachmentOptimal:
    case vk::ImageLayout::eDepthStencilReadOnlyOptimal:
    case vk::ImageLayout::eStencilReadOnlyOptimal:
        return vk::AccessFlagBits::eDepthStencilAttachmentRead;

    case vk::ImageLayout::eShaderReadOnlyOptimal:
        return vk::AccessFlagBits::eShaderRead
            | vk::AccessFlagBits::eInputAttachmentRead;

    case vk::ImageLayout::eTransferSrcOptimal:
        return vk::AccessFlagBits::eTransferRead;
    case vk::ImageLayout::eTransferDstOptimal:
        return vk::AccessFlagBits::eTransferWrite;
    case vk::ImageLayout::ePreinitialized:
        return vk::AccessFlagBits::eHostWrite
            | vk::AccessFlagBits::eTransferWrite;

    case vk::ImageLayout::eReadOnlyOptimal:
        break;
    case vk::ImageLayout::eAttachmentOptimal:
        break;
    case vk::ImageLayout::eVideoDecodeDstKHR:
        break;
    case vk::ImageLayout::eVideoDecodeSrcKHR:
        break;
    case vk::ImageLayout::eVideoDecodeDpbKHR:
        break;
    case vk::ImageLayout::eSharedPresentKHR:
        return vk::AccessFlagBits::eMemoryRead;
    case vk::ImageLayout::eFragmentDensityMapOptimalEXT:
        break;
    case vk::ImageLayout::eFragmentShadingRateAttachmentOptimalKHR:
        break;
    case vk::ImageLayout::eAttachmentFeedbackLoopOptimalEXT:
        break;
    default:
        throw std::runtime_error("Unknown Image Layout !");
    }
    throw std::runtime_error("Error: unsupported layout transition!");
    return vk::AccessFlagBits(-1);
}
