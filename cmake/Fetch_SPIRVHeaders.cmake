include_guard(GLOBAL)
include(${CMAKE_CURRENT_LIST_DIR}/Fetch_Common.cmake)

# SPIRV-Headers: same scheme as Vulkan-Headers (install once, find_package() everywhere).
#
# Provides SPIRV-Headers::SPIRV-Headers in the calling directory, plus SPIRV-Headers_SOURCE_DIR.
# SPIRV-Tools locates the headers through that variable and only uses <dir>/include, which the
# install prefix provides too, so no separate checkout is needed.
function(Fetch_SPIRVHeaders)
  MSG_InstallExternalPackage(SPIRV-Headers
    https://github.com/KhronosGroup/SPIRV-Headers.git ${MSG_VULKAN_SDK_TAG})
  find_package(SPIRV-Headers REQUIRED CONFIG PATHS "${MSG_EXTERNAL_PATH}" NO_DEFAULT_PATH)

  get_target_property(include_dirs SPIRV-Headers::SPIRV-Headers INTERFACE_INCLUDE_DIRECTORIES)
  list(GET include_dirs 0 include_dir)
  cmake_path(GET include_dir PARENT_PATH headers_root)
  set(SPIRV-Headers_SOURCE_DIR "${headers_root}" CACHE PATH "Root of the SPIRV-Headers install (contains include/)" FORCE)
endfunction()