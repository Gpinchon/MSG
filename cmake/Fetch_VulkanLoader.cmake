include_guard(GLOBAL)
include(FetchContent)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_Common.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_VulkanHeaders.cmake)

# Vulkan-Loader is linked by your targets, so it is built in-tree (parallel, incremental)
# rather than during configure. It finds Vulkan-Headers through find_package(), which
# Fetch_VulkanHeaders() has already satisfied.
#
# Provides the `vulkan` target, and Vulkan::Headers in the calling directory.
function(Fetch_VulkanLoader)
  Fetch_VulkanHeaders() # every caller needs the imported target in its own directory
  if(TARGET vulkan)
    return() # targets from add_subdirectory are global: later callers just reuse them
  endif()

  # Plain (non-cache) variables: the Loader's option() calls honour them (CMP0077) without
  # touching the cache, so a BUILD_TESTS option of your own project is left alone.
  set(UPDATE_DEPS OFF)
  set(BUILD_TESTS OFF)

  FetchContent_Declare(
    Vulkan-Loader
    GIT_REPOSITORY  https://github.com/KhronosGroup/Vulkan-Loader.git
    GIT_TAG         ${MSG_VULKAN_SDK_TAG}
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
  )
  FetchContent_MakeAvailable(Vulkan-Loader)
  FetchContent_GetProperties(Vulkan-Loader SOURCE_DIR source_dir)
  message(STATUS "Vulkan-Loader cloned to: ${source_dir}")

  if(COMMAND set_subdirectory_folder)
    set_subdirectory_folder("3rdParty/Vulkan-Loader" ${source_dir})
  endif()
endfunction()