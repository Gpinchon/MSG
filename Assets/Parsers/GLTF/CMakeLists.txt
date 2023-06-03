cmake_minimum_required(VERSION 3.19)

include(ExternalProject)

#Fetch NlohmannJson
FetchContent_Declare(
  NLOHMANN_JSON
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2
)
option(JSON_BuildTests                     "Build the unit tests when BUILD_TESTING is enabled." OFF)
FetchContent_MakeAvailable(NLOHMANN_JSON)
message(STATUS "Fetched NLOHMANN_JSON to ${nlohmann_json_SOURCE_DIR}")
set_subdirectory_folder("3rdparty/NlohmannJson" ${nlohmann_json_SOURCE_DIR})
#Fetch NlohmannJson

set(TABGRAPH_GLTF_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/GLTF.cpp)

add_library(TabGraph-Assets-Parsers-GLTF ${TABGRAPH_GLTF_DATA_SOURCE_FILES})
target_link_libraries(TabGraph-Assets-Parsers-GLTF
  TabGraph::Assets
  nlohmann_json)

set_target_properties(
  TabGraph-Assets-Parsers-GLTF
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} TabGraph-Assets-Parsers-GLTF PARENT_SCOPE)

#Mime-Extension
set(TABGRAPH_PARSER_MIME_EXTENSION0 ${TABGRAPH_PARSER_MIME_EXTENSION0} "model/gltf+json" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_EXTENSION1 ${TABGRAPH_PARSER_MIME_EXTENSION1} ".gltf" PARENT_SCOPE)

set(TABGRAPH_PARSER_MIME_FUNCTION0 ${TABGRAPH_PARSER_MIME_FUNCTION0} "model/gltf+json" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_FUNCTION1 ${TABGRAPH_PARSER_MIME_FUNCTION1} "ParseGLTF" PARENT_SCOPE)