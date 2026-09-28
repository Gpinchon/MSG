include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_VulkanLoader.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_VulkanValidationLayers.cmake)

# Uses the installed Vulkan SDK when there is one, otherwise fetches a minimal set
# (Loader + Headers + validation layers). Sets MSG_VK_LIBS in the caller's scope.
# Safe to call from any number of directories.
function(Fetch_VulkanSDK)
  find_package(Vulkan QUIET)
  if(Vulkan_FOUND)
    set(vk_libs Vulkan::Vulkan)
  else()
    get_property(announced GLOBAL PROPERTY MSG_VK_FETCH_ANNOUNCED)
    if(NOT announced)
      message(STATUS "Vulkan SDK not installed, fetching minimal dependencies")
      set_property(GLOBAL PROPERTY MSG_VK_FETCH_ANNOUNCED TRUE)
    endif()
    Fetch_VulkanValidationLayers()
    Fetch_VulkanLoader()
    set(vk_libs vulkan Vulkan::Headers)
  endif()
  set(MSG_VK_LIBS ${vk_libs} PARENT_SCOPE)
endfunction()