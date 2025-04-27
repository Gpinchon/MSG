macro(Fetch_NlohmannJson)
  if (NOT TARGET nlohmann_json)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
    FetchContent_Declare(
      NLOHMANN_JSON
      GIT_REPOSITORY https://github.com/nlohmann/json.git
      GIT_TAG        v3.11.2
    )
    option(JSON_BuildTests "Build the unit tests when BUILD_TESTING is enabled." OFF)
    FetchContent_MakeAvailable(NLOHMANN_JSON)
    message(STATUS "Fetched NLOHMANN_JSON to ${nlohmann_json_SOURCE_DIR}")
    set_subdirectory_folder("3rdparty/NlohmannJson" ${nlohmann_json_SOURCE_DIR})
  endif (NOT TARGET nlohmann_json)
endmacro()