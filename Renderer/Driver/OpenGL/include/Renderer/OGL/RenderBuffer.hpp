#pragma once

#include <Renderer/Handles.hpp>

#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Win32/Context.hpp>

namespace TabGraph::Renderer::RenderBuffer {
struct Impl : RAII::Wrapper<RAII::Texture2D> {
    Impl(
        RAII::Context& a_Context,
        const CreateRenderBufferInfo& a_Info);
};
}
