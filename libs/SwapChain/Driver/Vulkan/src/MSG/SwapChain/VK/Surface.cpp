#include <MSG/SwapChain/SwapChain.hpp>
#include <MSG/SwapChain/VK/Surface.hpp>
#include <MSG/VKInvoke.hpp>

#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#elif __linux__
#include <X11/Xlib.h>
#include <vulkan/vulkan_xlib.h>
#endif

VkSurfaceKHR CreateSurface(
    vk::raii::Instance& a_Instance,
    const Msg::SwapChain::CreateSwapChainInfo& a_Info)
{
    VkSurfaceKHR surface = nullptr;
#ifdef _WIN32
    VkWin32SurfaceCreateInfoKHR vkInfo { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    vkInfo.hwnd      = WindowFromDC(std::any_cast<HDC>(a_Info.windowInfo.nativeDisplayHandle));
    vkInfo.hinstance = (HINSTANCE)GetWindowLongPtr(vkInfo.hwnd, GWLP_HINSTANCE);
    vkCreateWin32SurfaceKHR(*a_Instance, &vkInfo, nullptr, &surface);
#elif __linux__
    VkXlibSurfaceCreateInfoKHR vkInfo { VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR };
    vkInfo.dpy    = std::any_cast<Display*>(a_Info.windowInfo.nativeDisplayHandle);
    vkInfo.window = std::any_cast<Window>(a_Info.windowInfo.nativeWindowHandle);
    VK_INVOKE(vkCreateXlibSurfaceKHR(*a_Instance, &vkInfo, nullptr, &surface));
#endif //_WIN32
    assert(surface != nullptr && "Surface creation failed !");
    return surface;
}

Msg::SwapChain::Surface::Surface(vk::raii::Instance& a_Instance, const CreateSwapChainInfo& a_Info)
    : vk::raii::SurfaceKHR(a_Instance, CreateSurface(a_Instance, a_Info))
{
}
