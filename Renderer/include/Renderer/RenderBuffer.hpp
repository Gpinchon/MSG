#pragma once

#include <Renderer/Handles.hpp>
#include <Renderer/Structs.hpp>

namespace TabGraph::Renderer::RenderBuffer {
Handle Create(const Renderer::Handle& a_Renderer, const CreateRenderBufferInfo& a_Info);
}