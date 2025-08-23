macro(Fetch_OGLRegistry)
  FetchContent_Declare(
      GL_REGISTRY
      GIT_REPOSITORY https://github.com/KhronosGroup/OpenGL-Registry/
      GIT_TAG        98c707a87ef63aae2ac8355abf59a6fb1823a6f3
  )
  FetchContent_MakeAvailable(GL_REGISTRY)
  set(GL_INCLUDE_DIRS
    "${gl_registry_SOURCE_DIR}/api")
endmacro()