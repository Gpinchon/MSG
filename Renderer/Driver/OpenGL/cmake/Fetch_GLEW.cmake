include(FetchContent)

# Fetch GLEW
macro(Fetch_GLEW)
  if (NOT TARGET GLEW::GLEW)
    FetchContent_Declare(
      GLEW
      GIT_REPOSITORY  https://github.com/Perlmint/glew-cmake.git
      GIT_TAG         glew-cmake-2.2.0
    )
    option(ONLY_LIBS "Do not build executables" ON)
    option(glew-cmake_BUILD_STATIC "Build the static glew library" ON)
    option(glew-cmake_BUILD_SHARED "Build the shared glew library" OFF)
    FetchContent_MakeAvailable(GLEW)
    add_library(GLEW::glew_s ALIAS libglew_static)
    add_library(GLEW::GLEW ALIAS libglew_static)
    set(GLEW_FOUND CACHE INTERNAL BOOL "GLEW found" ON)
    set(GLEW_INCLUDE_DIR ${glew_SOURCE_DIR}/include)
    set(GLEW_LIBRARIES GLEW::glew GLEW::glew_s)
    set_subdirectory_folder("3rdparty/GLEW" ${glew_SOURCE_DIR})
  ENDIF (NOT TARGET GLEW::GLEW)
endmacro()
