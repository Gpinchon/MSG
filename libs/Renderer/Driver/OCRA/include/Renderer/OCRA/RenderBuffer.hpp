#pragma once

#include <MSG/Renderer/RenderBuffer.hpp>
#include <MSG/Renderer/Renderer.hpp>

#include <OCRA/OCRA.hpp>

namespace MSG::Renderer::RenderBuffer {
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateRenderBufferInfo& a_Info);
    OCRA::Image::Handle image;
    OCRA::Image::View::Handle imageView;
    OCRA::uExtent2D extent;
};
}
