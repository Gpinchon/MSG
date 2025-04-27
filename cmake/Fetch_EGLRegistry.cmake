macro(Fetch_EGLRegistry)
  FetchContent_Declare(
      EGL_REGISTRY
      GIT_TAG        main
      GIT_REPOSITORY https://github.com/KhronosGroup/EGL-Registry.git
  )
  FetchContent_MakeAvailable(EGL_REGISTRY)
  set(EGL_INCLUDE_DIRS
    "${egl_registry_SOURCE_DIR}/api")
endmacro()