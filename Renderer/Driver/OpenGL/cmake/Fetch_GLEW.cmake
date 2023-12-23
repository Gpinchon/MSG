include(FetchContent)

# Fetch GLEW
macro(Fetch_GLEW)
  find_package(GLEW 2.2.0 QUIET)
  if (NOT GLEW_FOUND)
    if (WIN32)
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
    elseif (LINUX)
      FetchContent_Declare(
        GLEW
        URL https://codeload.github.com/nigels-com/glew/zip/refs/heads/master
      )
      FetchContent_GetProperties(GLEW)
      if (NOT glew_POPULATED)
        FetchContent_Populate(GLEW)
        set(GLEW_DEST "GLEW_DEST=${glew_BINARY_DIR}")
        set(GLEW_BUILD_VARS
          "GLEW_NO_GLU=-DGLEW_NO_GLU
          CC=${CMAKE_CXX_COMPILER}")
        execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} -C "${glew_SOURCE_DIR}/auto" ${GLEW_BUILD_VARS} ${GLEW_DEST})
        execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} -C "${glew_SOURCE_DIR}" ${GLEW_BUILD_VARS} ${GLEW_DEST})
        execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} -C "${glew_SOURCE_DIR}" install ${GLEW_BUILD_VARS} ${GLEW_DEST})
        execute_process(COMMAND ${CMAKE_MAKE_PROGRAM} -C "${glew_SOURCE_DIR}" clean)
        message("GLEW fetched to ${glew_SOURCE_DIR}")
      endif (NOT glew_POPULATED)

      list(APPEND CMAKE_PREFIX_PATH ${glew_BINARY_DIR})
      set(GLEW_USE_STATIC_LIBS true)
      find_package(GLEW 2.2.0 REQUIRED)

    endif (WIN32)
  ENDIF (NOT GLEW_FOUND)
  message("GLEW_INCLUDE_DIRS : ${GLEW_INCLUDE_DIRS}")
  message("GLEW_LIBRARIES    : ${GLEW_LIBRARIES}")
endmacro()
