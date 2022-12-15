#pragma once

#include <Renderer/Handle.hpp>

#include <string>

TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer);
TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer::Window);
TABGRAPH_RENDERER_HANDLE(TabGraph::Renderer::RenderBuffer);

namespace TabGraph::Renderer::Window {
struct Info {
    uint32_t width{ 0 }, height{ 0 };
    std::string name{ "" };
};
/**
* Creates a window and initialized the API for it
*/
Handle Create(const Renderer::Handle& a_Renderer, const Info& a_Info);
/**
* Initialized the API for an existing window
*/
Handle CreateFromNativeHandle(const Renderer::Handle& a_Renderer, void* a_NativeHandle);
/**
* @return the current back buffer of the window
*/
RenderBuffer::Handle GetBackBuffer(const Handle& a_Window);
/**
* @return an OS specific handle for the window
*/
void* GetNativeHandle(const Handle& a_Window);
}