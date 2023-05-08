#pragma once

#include <Renderer/Renderer.hpp>
#include <Renderer/RenderBuffer.hpp>

#include <OCRA/OCRA.hpp>

namespace TabGraph::Renderer::RenderBuffer {
struct Impl {
	Impl(
		const Renderer::Handle& a_Renderer,
		const CreateRenderBufferInfo& a_Info)
	{
		OCRA::CreateImageInfo imageInfo;
		imageInfo.extent.width  = a_Info.width;
		imageInfo.extent.height = a_Info.height;
		imageInfo.extent.depth  = 1;
		imageInfo.arrayLayers   = 1;
		imageInfo.format		= OCRA::Format::Uint8_Normalized_RGBA;
		imageInfo.mipLevels		= 1;
		imageInfo.type			= OCRA::ImageType::Image2D;
		imageInfo.usage			= OCRA::ImageUsageFlagBits::TransferSrc | OCRA::ImageUsageFlagBits::ColorAttachment;
		image = OCRA::Device::CreateImage(a_Renderer->logicalDevice, imageInfo);
		OCRA::CreateImageViewInfo imageViewInfo;
		imageViewInfo.format = OCRA::Format::Uint8_Normalized_RGBA;
		imageViewInfo.image  = image;
		imageViewInfo.type   = OCRA::ImageViewType::View2D;
		imageViewInfo.subRange.aspects = OCRA::ImageAspectFlagBits::Color;
		imageView = OCRA::Device::CreateImageView(a_Renderer->logicalDevice, imageViewInfo);
	}
	OCRA::Image::Handle image;
	OCRA::Image::View::Handle imageView;
};
}