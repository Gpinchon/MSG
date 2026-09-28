#pragma once

#ifdef MSG_DEBUG
#include <stdexcept>
#include <vulkan/vulkan.hpp>
constexpr static inline auto ToString(vk::Result& input_value)
{
    switch (input_value) {
    case vk::Result::eErrorCompressionExhaustedEXT:
        return "vk::Result::eErrorCompressionExhaustedEXT";
    case vk::Result::eErrorDeviceLost:
        return "vk::Result::eErrorDeviceLost";
    case vk::Result::eErrorExtensionNotPresent:
        return "vk::Result::eErrorExtensionNotPresent";
    case vk::Result::eErrorFeatureNotPresent:
        return "vk::Result::eErrorFeatureNotPresent";
    case vk::Result::eErrorFormatNotSupported:
        return "vk::Result::eErrorFormatNotSupported";
    case vk::Result::eErrorFragmentation:
        return "vk::Result::eErrorFragmentation";
    case vk::Result::eErrorFragmentedPool:
        return "vk::Result::eErrorFragmentedPool";
#if defined(VK_USE_PLATFORM_WIN32_KHR)
    case vk::Result::eErrorFullScreenExclusiveModeLostEXT:
#endif
        return "vk::Result::eErrorFullScreenExclusiveModeLostEXT";
    case vk::Result::eErrorImageUsageNotSupportedKHR:
        return "vk::Result::eErrorImageUsageNotSupportedKHR";
    case vk::Result::eErrorIncompatibleDisplayKHR:
        return "vk::Result::eErrorIncompatibleDisplayKHR";
    case vk::Result::eErrorIncompatibleDriver:
        return "vk::Result::eErrorIncompatibleDriver";
    case vk::Result::eErrorInitializationFailed:
        return "vk::Result::eErrorInitializationFailed";
    case vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT:
        return "vk::Result::eErrorInvalidDrmFormatModifierPlaneLayoutEXT";
    case vk::Result::eErrorInvalidExternalHandle:
        return "vk::Result::eErrorInvalidExternalHandle";
    case vk::Result::eErrorInvalidOpaqueCaptureAddress:
        return "vk::Result::eErrorInvalidOpaqueCaptureAddress";
    case vk::Result::eErrorInvalidShaderNV:
        return "vk::Result::eErrorInvalidShaderNV";
#ifdef VK_ENABLE_BETA_EXTENSIONS
    case vk::Result::eErrorInvalidVideoStdParametersKHR:
        return "vk::Result::eErrorInvalidVideoStdParametersKHR";
#endif // VK_ENABLE_BETA_EXTENSIONS
    case vk::Result::eErrorLayerNotPresent:
        return "vk::Result::eErrorLayerNotPresent";
    case vk::Result::eErrorMemoryMapFailed:
        return "vk::Result::eErrorMemoryMapFailed";
    case vk::Result::eErrorNativeWindowInUseKHR:
        return "vk::Result::eErrorNativeWindowInUseKHR";
    case vk::Result::eErrorNotPermittedKHR:
        return "vk::Result::eErrorNotPermittedKHR";
    case vk::Result::eErrorOutOfDateKHR:
        return "vk::Result::eErrorOutOfDateKHR";
    case vk::Result::eErrorOutOfDeviceMemory:
        return "vk::Result::eErrorOutOfDeviceMemory";
    case vk::Result::eErrorOutOfHostMemory:
        return "vk::Result::eErrorOutOfHostMemory";
    case vk::Result::eErrorOutOfPoolMemory:
        return "vk::Result::eErrorOutOfPoolMemory";
    case vk::Result::eErrorSurfaceLostKHR:
        return "vk::Result::eErrorSurfaceLostKHR";
    case vk::Result::eErrorTooManyObjects:
        return "vk::Result::eErrorTooManyObjects";
    case vk::Result::eErrorUnknown:
        return "vk::Result::eErrorUnknown";
    case vk::Result::eErrorValidationFailedEXT:
        return "vk::Result::eErrorValidationFailedEXT";
    case vk::Result::eErrorVideoPictureLayoutNotSupportedKHR:
        return "vk::Result::eErrorVideoPictureLayoutNotSupportedKHR";
    case vk::Result::eErrorVideoProfileCodecNotSupportedKHR:
        return "vk::Result::eErrorVideoProfileCodecNotSupportedKHR";
    case vk::Result::eErrorVideoProfileFormatNotSupportedKHR:
        return "vk::Result::eErrorVideoProfileFormatNotSupportedKHR";
    case vk::Result::eErrorVideoProfileOperationNotSupportedKHR:
        return "vk::Result::eErrorVideoProfileOperationNotSupportedKHR";
    case vk::Result::eErrorVideoStdVersionNotSupportedKHR:
        return "vk::Result::eErrorVideoStdVersionNotSupportedKHR";
    case vk::Result::eEventReset:
        return "vk::Result::eEventReset";
    case vk::Result::eEventSet:
        return "vk::Result::eEventSet";
    case vk::Result::eIncomplete:
        return "vk::Result::eIncomplete";
    case vk::Result::eNotReady:
        return "vk::Result::eNotReady";
    case vk::Result::eOperationDeferredKHR:
        return "vk::Result::eOperationDeferredKHR";
    case vk::Result::eOperationNotDeferredKHR:
        return "vk::Result::eOperationNotDeferredKHR";
    case vk::Result::ePipelineCompileRequired:
        return "vk::Result::ePipelineCompileRequired";
    case vk::Result::eSuboptimalKHR:
        return "vk::Result::eSuboptimalKHR";
    case vk::Result::eSuccess:
        return "vk::Result::eSuccess";
    case vk::Result::eThreadDoneKHR:
        return "vk::Result::eThreadDoneKHR";
    case vk::Result::eThreadIdleKHR:
        return "vk::Result::eThreadIdleKHR";
    case vk::Result::eTimeout:
        return "vk::Result::eTimeout";
    default:
        return "Unhandled VkResult";
    }
}
#define VK_CHECK(result)                                \
    {                                                   \
        if (result != vk::Result::eSuccess) {           \
            throw std::runtime_error(ToString(result)); \
        }                                               \
    }
#define VK_INVOKE(function)  \
    {                        \
        auto res = function; \
        VK_CHECK(res);       \
    }
#else
#define VK_CHECK(result)
#define VK_INVOKE(function)     \
    {                           \
        std::ignore = function; \
    }
#endif