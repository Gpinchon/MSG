include_guard(GLOBAL)
include(FetchContent)

# SPIRV-Cross, built in-tree as static libraries.
function(Fetch_SPIRVCross)
  if(TARGET spirv-cross-core)
    return() # targets from add_subdirectory are global: later callers just reuse them
  endif()

  # Names are specific to SPIRV-Cross, so forcing them can't clobber unrelated options.
  set(SPIRV_CROSS_SKIP_INSTALL              ON  CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_EXCEPTIONS_TO_ASSERTIONS  OFF CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_SHARED                    OFF CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_STATIC                    ON  CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_CLI                       OFF CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_TESTS              OFF CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_GLSL               ON  CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_HLSL               ON  CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_MSL                ON  CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_CPP                ON  CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_REFLECT            ON  CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_C_API              OFF CACHE BOOL "" FORCE)
  set(SPIRV_CROSS_ENABLE_UTIL               OFF CACHE BOOL "" FORCE)

  FetchContent_Declare(
    SPIRV-Cross
    GIT_REPOSITORY  https://github.com/KhronosGroup/SPIRV-Cross.git
    GIT_TAG         vulkan-sdk-1.4.357.0
    GIT_SHALLOW     TRUE
    EXCLUDE_FROM_ALL
    SYSTEM
  )
  FetchContent_MakeAvailable(SPIRV-Cross)
  FetchContent_GetProperties(SPIRV-Cross SOURCE_DIR source_dir)
  message(STATUS "SPIRV-Cross cloned to: ${source_dir}")

  if(COMMAND set_subdirectory_folder)
    set_subdirectory_folder("3rdParty/SPIRV-Cross" ${source_dir})
  endif()
endfunction()