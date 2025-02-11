include(FetchContent)

function(BuildGLEW a_BuildType)
  message("Building GLEW::${a_BuildType}")
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      "-DONLY_LIBS=ON"
      "-Dglew-cmake_BUILD_STATIC=ON"
      "-Dglew-cmake_BUILD_SHARED=OFF"
      -G ${CMAKE_GENERATOR}
      -S ${glew_SOURCE_DIR}
      -B ${glew_BINARY_DIR}
      --install-prefix ${CMAKE_BINARY_DIR}/external)
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --build ${glew_BINARY_DIR}
      --config ${a_BuildType})
  execute_process(
    COMMAND ${CMAKE_COMMAND}
      --install ${glew_BINARY_DIR}
      --install-prefix ${CMAKE_BINARY_DIR}/external
      --config ${a_BuildType})
endfunction()

# Fetch GLEW
macro(Fetch_GLEW)
  option(GLEW_USE_STATIC_LIBS "" TRUE)
  find_package(GLEW 2.2.0 QUIET)
  if (NOT GLEW_FOUND)
    if (WIN32)
      FetchContent_Declare(
        GLEW
        GIT_REPOSITORY  https://github.com/Perlmint/glew-cmake.git
        GIT_TAG         glew-cmake-2.2.0
      )
      FetchContent_GetProperties(GLEW)
      if (NOT glew_POPULATED)
        FetchContent_Populate(GLEW)
        BuildGLEW(Release)
      endif(NOT glew_POPULATED)
    elseif (LINUX)
      FetchContent_Declare(
        GLEW
        GIT_REPOSITORY  https://github.com/nigels-com/glew.git
        GIT_TAG         b323ebf9adeae6a3f26f91277d4f62df509037fc
      )
      FetchContent_GetProperties(GLEW)
      if (NOT glew_POPULATED)
        FetchContent_Populate(GLEW)
        set(GLEW_DEST "GLEW_DEST=${CMAKE_BINARY_DIR}/external")
        set(GLEW_BUILD_VARS "GLEW_NO_GLU=-DGLEW_NO_GLU CC=${CMAKE_CXX_COMPILER} SYSTEM=linux-egl")
        execute_process(
          COMMAND make extensions ${GLEW_BUILD_VARS} ${GLEW_DEST}
          WORKING_DIRECTORY ${glew_SOURCE_DIR})
        execute_process(
          COMMAND make ${GLEW_BUILD_VARS} ${GLEW_DEST}
          WORKING_DIRECTORY ${glew_SOURCE_DIR})
        execute_process(
          COMMAND make install ${GLEW_BUILD_VARS} ${GLEW_DEST}
          WORKING_DIRECTORY ${glew_SOURCE_DIR})
        execute_process(
          COMMAND make clean
          WORKING_DIRECTORY ${glew_SOURCE_DIR})
        message("GLEW fetched to ${glew_SOURCE_DIR}")
      endif (NOT glew_POPULATED)
      list(APPEND CMAKE_PREFIX_PATH ${glew_BINARY_DIR})
    endif (WIN32)
    find_package(GLEW 2.2.0 REQUIRED)
  ENDIF (NOT GLEW_FOUND)
  find_package(OpenGL REQUIRED)
  set(OPENGL_LIBRARIES GLEW::GLEW OpenGL::GL)
  message("GLEW_INCLUDE_DIRS     : ${GLEW_INCLUDE_DIRS}")
  message("GLEW_STATIC_LIBRARIES : ${GLEW_STATIC_LIBRARIES}")
  message("GLEW_SHARED_LIBRARIES : ${GLEW_SHARED_LIBRARIES}")
  message("OPENGL_LIBRARIES      : ${OPENGL_LIBRARIES}")
endmacro()
