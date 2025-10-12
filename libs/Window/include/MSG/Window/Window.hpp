#pragma once

#include <MSG/Renderer/Handles.hpp>
#include <MSG/SwapChain/Handles.hpp>
#include <MSG/Window/Handles.hpp>
#include <MSG/Window/Structs.hpp>

#include <any>

namespace Msg::Window {
Handle Create(const Renderer::Handle& a_Renderer, const CreateWindowInfo& a_Info);
void WaitSwapChain(const Handle& a_Window);
void Show(const Handle& a_Window);
void Present(const Handle& a_Window, const Renderer::RenderBuffer::Handle& a_RenderBuffer);
bool IsClosing(const Handle& a_Window);
uint32_t GetWidth(const Handle& a_Window);
uint32_t GetHeight(const Handle& a_Window);
SwapChain::Handle GetSwapChain(const Handle& a_Window);
std::any GetNativeWindowHandle(const Handle& a_Window);
std::any GetNativeDisplayHandle(const Handle& a_Window);
}
