include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_Common.cmake)

# Vulkan-Headers: installed once into MSG_EXTERNAL_PATH, then consumed through find_package()
# like any other package. Going through a real package also defines VulkanHeaders_VERSION,
# which Vulkan-Loader's own CMake needs (https://github.com/KhronosGroup/Vulkan-Loader/pull/1984),
# and VulkanHeaders_DIR, which lets the Loader's find_package(VulkanHeaders) find this copy.
#
# Provides Vulkan::Headers in the calling directory. Safe to call from any number of directories.
function(Fetch_VulkanHeaders)
  MSG_InstallExternalPackage(Vulkan-Headers
    https://github.com/KhronosGroup/Vulkan-Headers.git ${MSG_VULKAN_SDK_TAG})
  find_package(VulkanHeaders REQUIRED CONFIG PATHS "${MSG_EXTERNAL_PATH}" NO_DEFAULT_PATH)
endfunction()