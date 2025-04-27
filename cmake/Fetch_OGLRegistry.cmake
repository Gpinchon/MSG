macro(Fetch_OGLRegistry)
  FetchContent_Declare(
      GL_REGISTRY
      GIT_TAG        main
      GIT_REPOSITORY https://github.com/KhronosGroup/OpenGL-Registry/
  )
  FetchContent_MakeAvailable(GL_REGISTRY)
  set(GL_INCLUDE_DIRS
    "${gl_registry_SOURCE_DIR}/api")
endmacro()