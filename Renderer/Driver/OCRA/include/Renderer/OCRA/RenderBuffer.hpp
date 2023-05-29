#pragma once

#include <Renderer/RenderBuffer.hpp>
#include <Renderer/Renderer.hpp>

#include <OCRA/OCRA.hpp>

namespace TabGraph::Renderer::RenderBuffer {
struct Impl {
    Impl(
        const Renderer::Handle& a_Renderer,
        const CreateRenderBufferInfo& a_Info);
    OCRA::Image::Handle image;
    OCRA::Image::View::Handle imageView;
};
}
