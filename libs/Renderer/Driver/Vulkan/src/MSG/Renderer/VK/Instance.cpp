#include <MSG/Debug.hpp>
#include <MSG/Renderer/Structs.hpp>
#include <MSG/Renderer/VK/Instance.hpp>
#include <MSG/Renderer/VK/Invoke.hpp>

#ifdef _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#elif __linux__
#include <X11/Xlib.h>
#include <vulkan/vulkan_xlib.h>
#endif

#define ENGINE_VERSION 100
#define ENGINE_NAME    "MSG"

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
};

bool CheckValidationLayerSupport()
{

    uint32_t layerCount;
    VK_INVOKE(vk::enumerateInstanceLayerProperties(&layerCount, nullptr));
    std::vector<vk::LayerProperties> availableLayers(layerCount);
    VK_INVOKE(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()));
    for (const char* layerName : validationLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            return false;
        }
    }
    return true;
}

void PrintAvailableLayers()
{
    uint32_t layerCount;
    VK_INVOKE(vk::enumerateInstanceLayerProperties(&layerCount, nullptr));
    std::vector<vk::LayerProperties> availableLayers(layerCount);
    VK_INVOKE(vk::enumerateInstanceLayerProperties(&layerCount, availableLayers.data()));
    for (const auto& layerProperties : availableLayers) {
        MSGDebugLog(layerProperties.layerName.data());
    }
}

vk::raii::Instance Msg::Renderer::CreateInstance(const CreateRendererInfo& a_Info)
{
    const std::vector<const char*> extensions {
#ifdef _WIN32
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif __linux__
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#endif
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef MSG_DEBUG
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
    };
    static vk::raii::Context context;
    vk::InstanceCreateInfo info;
    vk::ApplicationInfo appInfo;
    appInfo.applicationVersion   = a_Info.applicationVersion;
    appInfo.pApplicationName     = a_Info.name.c_str();
    appInfo.engineVersion        = ENGINE_VERSION;
    appInfo.pEngineName          = ENGINE_NAME;
    appInfo.apiVersion           = VK_API_VERSION_1_3;
    info.pApplicationInfo        = &appInfo;
    info.enabledExtensionCount   = extensions.size();
    info.ppEnabledExtensionNames = extensions.data();
#ifdef MSG_DEBUG
    if (!CheckValidationLayerSupport()) {
        MSGErrorWarning("!!! VK_LAYER_KHRONOS_validation NOT SUPPORTED !!!");
        MSGErrorWarning("Available layers :");
        PrintAvailableLayers();
    } else {
        info.enabledLayerCount   = static_cast<uint32_t>(validationLayers.size());
        info.ppEnabledLayerNames = validationLayers.data();
    }
#endif
    return vk::raii::Instance(context, info);
}