cmake_minimum_required(VERSION 3.19)

include(ExternalProject)

FetchContent_Declare(
  RAPIDJSON
  GIT_REPOSITORY https://github.com/Tencent/rapidjson.git
  GIT_TAG        v1.1.0
)
FetchContent_MakeAvailable(RAPIDJSON)
message(STATUS "Fetched RAPIDJSON to ${rapidjson_SOURCE_DIR}")
set(RAPIDJSON_ROOT PATH "${rapidjson_SOURCE_DIR}")
# Prepare RapidJSON (RapidJSON is a header-only library)
set(RAPIDJSON_INCLUDE_DIR ${rapidjson_SOURCE_DIR}/include)
set_subdirectory_folder("3rdparty/RapidJSON" ${rapidjson_SOURCE_DIR})

set(TABGRAPH_GLTF_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/GLTF.cpp)

add_library(TabGraph-Assets-Parsers-GLTF ${TABGRAPH_GLTF_DATA_SOURCE_FILES})
target_include_directories(TabGraph-Assets-Parsers-GLTF PRIVATE ${RAPIDJSON_INCLUDE_DIR})
target_link_libraries(TabGraph-Assets-Parsers-GLTF TabGraph::Assets)

set_target_properties(
  TabGraph-Assets-Parsers-GLTF
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} TabGraph-Assets-Parsers-GLTF PARENT_SCOPE)
set(TABGRAPH_PARSER_FUNCTIONS ${TABGRAPH_PARSER_FUNCTIONS} "ParseGLTF" PARENT_SCOPE)

#Mime-Extension
set(TABGRAPH_PARSER_MIME_EXTENSION0 ${TABGRAPH_PARSER_MIME_EXTENSION0} "model/gltf+json" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_EXTENSION1 ${TABGRAPH_PARSER_MIME_EXTENSION1} ".gltf" PARENT_SCOPE)

set(TABGRAPH_PARSER_MIME_FUNCTION0 ${TABGRAPH_PARSER_MIME_FUNCTION0} "model/gltf+json" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_FUNCTION1 ${TABGRAPH_PARSER_MIME_FUNCTION1} "ParseGLTF" PARENT_SCOPE)