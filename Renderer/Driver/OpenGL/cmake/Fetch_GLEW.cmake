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
      set(GLEW_INCLUDE_DIRS ${glew_SOURCE_DIR}/include)
      set(GLEW_STATIC_LIBRARIES GLEW::glew_s)
      set(GLEW_SHARED_LIBRARIES GLEW::glew)
      set_subdirectory_folder("3rdparty/GLEW" ${glew_SOURCE_DIR})
      #[[
      FetchContent_Declare(
        GLEW
        URL  https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip
      )
      FetchContent_MakeAvailable(GLEW)
      message("GLEW fetched to ${glew_SOURCE_DIR}")
      list(APPEND CMAKE_PREFIX_PATH ${glew_SOURCE_DIR})
      set(GLEW_USE_STATIC_LIBS TRUE)
      find_package(GLEW 2.2.0 REQUIRED)
      ]]
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
