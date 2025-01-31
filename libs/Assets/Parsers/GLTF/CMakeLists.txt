cmake_minimum_required(VERSION 3.19)

#Fetch NlohmannJson
FetchContent_Declare(
  NLOHMANN_JSON
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2
)
option(JSON_BuildTests "Build the unit tests when BUILD_TESTING is enabled." OFF)
FetchContent_MakeAvailable(NLOHMANN_JSON)
message(STATUS "Fetched NLOHMANN_JSON to ${nlohmann_json_SOURCE_DIR}")
set_subdirectory_folder("3rdparty/NlohmannJson" ${nlohmann_json_SOURCE_DIR})
#Fetch NlohmannJson

set(MSG_GLTF_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/GLTF.cpp)

add_library(MSG-Assets-Parsers-GLTF ${MSG_GLTF_DATA_SOURCE_FILES})
target_link_libraries(MSG-Assets-Parsers-GLTF
  MSG::Assets
  MSG::PrimitiveOptimizer
  nlohmann_json)

set_target_properties(
  MSG-Assets-Parsers-GLTF
  PROPERTIES FOLDER "MSG/Assets/Parsers")

set(MSG_PARSER_LIBS ${MSG_PARSER_LIBS} MSG-Assets-Parsers-GLTF PARENT_SCOPE)

#Mime-Extension
set(MSG_PARSER_MIME_EXTENSION0 ${MSG_PARSER_MIME_EXTENSION0} "model/gltf+json" PARENT_SCOPE)
set(MSG_PARSER_MIME_EXTENSION1 ${MSG_PARSER_MIME_EXTENSION1} ".gltf" PARENT_SCOPE)

set(MSG_PARSER_MIME_FUNCTION0 ${MSG_PARSER_MIME_FUNCTION0} "model/gltf+json" PARENT_SCOPE)
set(MSG_PARSER_MIME_FUNCTION1 ${MSG_PARSER_MIME_FUNCTION1} "ParseGLTF" PARENT_SCOPE)