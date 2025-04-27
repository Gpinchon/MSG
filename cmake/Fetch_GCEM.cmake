macro(Fetch_GCEM)
  if (NOT TARGET gcem)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    FetchContent_Declare(
    GCEM
    GIT_REPOSITORY https://github.com/kthohr/gcem.git
    GIT_TAG        v1.16.0
    )
    FetchContent_MakeAvailable(GCEM)
    message(STATUS "Fetched GCEM to ${gcem_SOURCE_DIR}")
    set_subdirectory_folder("3rdparty/GCEM" ${gcem_SOURCE_DIR})
  endif (NOT TARGET gcem)
endmacro()