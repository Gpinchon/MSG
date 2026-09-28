include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_Common.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_SPIRVHeaders.cmake)

function(Fetch_SPIRVTools)
  if(TARGET SPIRV-Tools-static)
    return() # targets from add_subdirectory are global: later callers just reuse them
  endif()

  Fetch_SPIRVHeaders() # defines SPIRV-Headers_SOURCE_DIR, read by SPIRV-Tools' build

  set(SPIRV_SKIP_TESTS ON  CACHE BOOL "" FORCE) # no GoogleTest needed
  set(SPIRV_WERROR     OFF CACHE BOOL "" FORCE) # replaces -Wno-error=array-bounds (GCC false positive)

  FetchContent_Declare(
    SPIRV-Tools
    GIT_REPOSITORY  https://github.com/KhronosGroup/SPIRV-Tools.git
    GIT_TAG         ${MSG_VULKAN_SDK_TAG}
    GIT_SHALLOW     TRUE
    OVERRIDE_FIND_PACKAGE # in-tree find_package(SPIRV-Tools) calls resolve to this copy
    EXCLUDE_FROM_ALL
    SYSTEM
  )
  FetchContent_MakeAvailable(SPIRV-Tools)
  FetchContent_GetProperties(SPIRV-Tools SOURCE_DIR source_dir)

  if(NOT TARGET SPIRV-Tools-static)
    message(FATAL_ERROR "SPIRV-Tools was fetched to ${source_dir} but SPIRV-Tools-static was not created")
  endif()
  message(STATUS "SPIRV-Tools cloned to: ${source_dir}")
  if(COMMAND set_subdirectory_folder)
    set_subdirectory_folder("3rdParty/Vulkan-Loader" ${source_dir})
  endif()
endfunction()