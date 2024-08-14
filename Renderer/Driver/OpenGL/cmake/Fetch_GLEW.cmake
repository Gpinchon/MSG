include(FetchContent)

# Fetch GLEW
macro(Fetch_GLEW)
  find_package(GLEW 2.2.0 QUIET)
  if (NOT GLEW_FOUND)
    if (WIN32)
      FetchContent_Declare(
        GLEW
        URL  https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
      )
      FetchContent_MakeAvailable(GLEW)
      message("GLEW fetched to ${glew_SOURCE_DIR}")
      list(APPEND CMAKE_PREFIX_PATH ${glew_SOURCE_DIR})
      find_package(GLEW 2.2.0 REQUIRED)
    elseif (LINUX)
      FetchContent_Declare(
        GLEW
        GIT_REPOSITORY  https://github.com/nigels-com/glew.git
        GIT_TAG         b323ebf9adeae6a3f26f91277d4f62df509037fc
      )
      FetchContent_GetProperties(GLEW)
      if (NOT glew_POPULATED)
        FetchContent_Populate(GLEW)
        set(GLEW_DEST "GLEW_DEST=${glew_BINARY_DIR}")
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
      find_package(GLEW 2.2.0 REQUIRED)
      
    endif (WIN32)
  ENDIF (NOT GLEW_FOUND)
  message("GLEW_INCLUDE_DIRS     : ${GLEW_INCLUDE_DIRS}")
  message("GLEW_STATIC_LIBRARIES : ${GLEW_STATIC_LIBRARIES}")
  message("GLEW_SHARED_LIBRARIES : ${GLEW_SHARED_LIBRARIES}")
endmacro()
