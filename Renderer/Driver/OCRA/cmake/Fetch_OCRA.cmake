include(FetchContent)

# Fetch OCRA
macro(Fetch_OCRA)
  FetchContent_Declare(
    OCRA
    GIT_REPOSITORY https://github.com/Gpinchon/OCRA.git
    GIT_TAG        origin/main
  )
  FetchContent_MakeAvailable(OCRA)
  message(STATUS "Fetched OCRA to ${ocra_SOURCE_DIR}")
  set_subdirectory_folder("3rdparty/OCRA" ${ocra_SOURCE_DIR})
endmacro()
