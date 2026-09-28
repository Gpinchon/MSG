#include <MSG/Debug.hpp>
#include <MSG/VKInstance.hpp>
#include <MSG/VKInvoke.hpp>

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

static bool CheckValidationLayerSupport(const vk::raii::Context& a_Ctx)
{
    auto availableLayers = a_Ctx.enumerateInstanceLayerProperties();
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

static void PrintAvailableLayers(const vk::raii::Context& a_Ctx)
{
    for (const auto& layerProperties : a_Ctx.enumerateInstanceLayerProperties()) {
        MSGDebugLog(layerProperties.layerName.data());
    }
}

static vk::raii::Instance CreateInstance(const vk::raii::Context& a_Ctx, const std::string& a_AppName, const uint32_t& a_AppVersion)
{
    vk::InstanceCreateInfo info;
    vk::ApplicationInfo appInfo;
    appInfo.applicationVersion = a_AppVersion;
    appInfo.pApplicationName   = a_AppName.c_str();
    appInfo.engineVersion      = ENGINE_VERSION;
    appInfo.pEngineName        = ENGINE_NAME;
    appInfo.apiVersion         = VK_API_VERSION_1_3;
    info.setPApplicationInfo(&appInfo);
    info.setPEnabledExtensionNames(extensions);
#ifdef MSG_DEBUG
    if (!CheckValidationLayerSupport(a_Ctx)) {
        MSGErrorWarning("!!! VK_LAYER_KHRONOS_validation NOT SUPPORTED !!!");
        MSGErrorWarning("Available layers :");
        PrintAvailableLayers(a_Ctx);
    } else {
        info.setPEnabledLayerNames(validationLayers);
    }
#endif
    return a_Ctx.createInstance(info);
}

Msg::VKInstance::VKInstance(const std::string& a_AppName, const uint32_t& a_AppVersion)
    : instance(CreateInstance(context, a_AppName, a_AppVersion))
{
}

std::vector<vk::LayerProperties> Msg::VKInstance::GetAvailableLayers() const
{
    return context.enumerateInstanceLayerProperties();
}
