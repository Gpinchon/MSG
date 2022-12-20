#pragma once

#include <OCRA/Image/Image.hpp>

namespace TabGraph::Renderer::RenderBuffer {
struct Impl {
	Impl(const OCRA::Image::Handle& a_Image) : image(a_Image) {}
	OCRA::Image::Handle image;
};
}