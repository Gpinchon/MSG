include_guard(GLOBAL)
include(ExternalProject)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_Common.cmake)

# Vulkan-ValidationLayers is a runtime plugin: nothing links against it, so it doesn't have
# to exist at configure time. ExternalProject_Add builds and installs it as part of the normal
# build graph (part of ALL), into MSG_EXTERNAL_PATH.
#
# Its own UPDATE_DEPS option makes it fetch and build the dependencies it needs (Vulkan-Headers,
# SPIRV-Headers/Tools, ...) at the versions it was released with, so none of the other Fetch_*
# modules have to install anything for it.
#
# Installed layout:
#   Windows : VkLayer_khronos_validation.dll + .json  -> <prefix>/bin
#   Linux   : libVkLayer_khronos_validation.so        -> <prefix>/lib
#             VkLayer_khronos_validation.json         -> <prefix>/share/vulkan/explicit_layer.d
# VK_LAYER_PATH must point at the directory holding the *manifest* (.json).
#
# Outputs (CACHE):
#   MSG_VK_LAYER_PATH  directory containing the layer manifest (-> VK_LAYER_PATH)
#   MSG_DEBUGGER_ENV   list of NAME=VALUE entries for the debugged process' environment
function(Fetch_VulkanValidationLayers)
  MSG_RequireExternalPath()

  if(WIN32)
    set(manifest_dir "${MSG_EXTERNAL_PATH}/bin")
    set(library_dir  "${MSG_EXTERNAL_PATH}/bin")
  else()
    set(manifest_dir "${MSG_EXTERNAL_PATH}/share/vulkan/explicit_layer.d")
    set(library_dir  "${MSG_EXTERNAL_PATH}/lib")
  endif()

  # If the caller pointed MSG_VK_LAYER_PATH somewhere else (e.g. a Vulkan SDK), don't build ours.
  if(DEFINED MSG_VK_LAYER_PATH AND NOT "${MSG_VK_LAYER_PATH}" STREQUAL "${manifest_dir}")
    if(NOT DEFINED MSG_DEBUGGER_ENV)
      set(MSG_DEBUGGER_ENV "VK_LAYER_PATH=${MSG_VK_LAYER_PATH}" CACHE STRING "The debugging environment")
    endif()
    return()
  endif()

  # ExternalProject targets are global: declare once per configure, whichever directory gets here first.
  if(NOT TARGET Vulkan-ValidationLayers)
    set(vvl_cache_args
      -DCMAKE_BUILD_TYPE:STRING=Release
      -DCMAKE_C_COMPILER:FILEPATH=${CMAKE_C_COMPILER}
      -DCMAKE_CXX_COMPILER:FILEPATH=${CMAKE_CXX_COMPILER}
      -DCMAKE_INSTALL_PREFIX:PATH=${MSG_EXTERNAL_PATH}
      -DCMAKE_INSTALL_LIBDIR:STRING=lib # deterministic layout
      -DUPDATE_DEPS:BOOL=ON
      -DBUILD_TESTS:BOOL=OFF
      -DBUILD_WERROR:BOOL=OFF)
    if(CMAKE_TOOLCHAIN_FILE)
      list(APPEND vvl_cache_args -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
    endif()

    ExternalProject_Add(
      Vulkan-ValidationLayers
      GIT_REPOSITORY   https://github.com/KhronosGroup/Vulkan-ValidationLayers.git
      GIT_TAG          ${MSG_VULKAN_SDK_TAG}
      GIT_SHALLOW      TRUE
      CMAKE_GENERATOR  "${CMAKE_GENERATOR}"
      CMAKE_CACHE_ARGS ${vvl_cache_args}
      BUILD_COMMAND    ${CMAKE_COMMAND} --build <BINARY_DIR> --config Release --parallel
      INSTALL_COMMAND  ${CMAKE_COMMAND} --install <BINARY_DIR> --config Release
      BUILD_BYPRODUCTS "${manifest_dir}/VkLayer_khronos_validation.json"
      USES_TERMINAL_BUILD   TRUE
      USES_TERMINAL_INSTALL TRUE
    )
    message(STATUS "Vulkan-ValidationLayers will be built to ${MSG_EXTERNAL_PATH}")
  endif()

  
  if(NOT WIN32)
    # The manifest's library_path is typically a bare .so name, resolved through the dynamic loader.
    set(ld_path "${library_dir}")
    if(NOT "$ENV{LD_LIBRARY_PATH}" STREQUAL "")
      string(APPEND ld_path ":$ENV{LD_LIBRARY_PATH}")
    endif()
    list(APPEND lib_path "LD_LIBRARY_PATH=${ld_path}")
    MSG_Append_Debugger_Env(${lib_path})
  endif()

  set(MSG_VK_LAYER_PATH "${manifest_dir}" CACHE PATH "Home of Vulkan layers" FORCE)
  MSG_Append_Debugger_Env("VK_LAYER_PATH=${manifest_dir}")
endfunction()
