include(FetchContent)

macro(Fetch_FasTC)
  FetchContent_Declare(
    FASTC
    GIT_REPOSITORY  https://github.com/Gpinchon/FasTC.git
  )
  FetchContent_MakeAvailable(FASTC)
  message(STATUS "Fetched FASTC to ${fastc_SOURCE_DIR}")
  set_subdirectory_folder("3rdparty/FasTC" ${fastc_SOURCE_DIR})
  get_target_property(FASTCBASE_INCLUDES FasTCBase INCLUDE_DIRECTORIES)
  get_target_property(FASTCCORE_INCLUDES FasTCCore INCLUDE_DIRECTORIES)
endmacro()
