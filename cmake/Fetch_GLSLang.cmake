include(FetchContent)

# Fetch GLSLANG
macro(Fetch_GLSLang)
  FetchContent_Declare(
    GLSLANG
    GIT_REPOSITORY  https://github.com/KhronosGroup/glslang.git
    GIT_TAG         sdk-1.3.239.0
  )
  option(SKIP_GLSLANG_INSTALL "Skip Installation" ON)
  FetchContent_MakeAvailable(GLSLANG)
  set_subdirectory_folder("3rdParty/GLSLang" ${glslang_SOURCE_DIR})
endmacro()
