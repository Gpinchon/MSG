cmake_minimum_required(VERSION 3.19)

Fetch_ZLib()

set(TABGRAPH_FBX_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXDocument.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXNode.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXObject.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FBX/FBXProperty.hpp)

set(TABGRAPH_FBX_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBX.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXDocument.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXNode.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXObject.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FBXProperty.cpp)

add_library(TabGraph-Assets-Parsers-FBX ${TABGRAPH_FBX_HEADER_FILES} ${TABGRAPH_FBX_SOURCE_FILES})
target_link_libraries(TabGraph-Assets-Parsers-FBX TabGraph::Assets)

set_target_properties(
  TabGraph-Assets-Parsers-FBX
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)
target_include_directories(TabGraph-Assets-Parsers-FBX PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${ZLIB_INCLUDE_DIR}")
target_link_libraries(TabGraph-Assets-Parsers-FBX ZLIB::ZLIB)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} TabGraph-Assets-Parsers-FBX PARENT_SCOPE)

#Not standard mime but fuck it.
set(TABGRAPH_PARSER_MIME_EXTENSION0 ${TABGRAPH_PARSER_MIME_EXTENSION0} "model/fbx" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_EXTENSION1 ${TABGRAPH_PARSER_MIME_EXTENSION1} ".fbx" PARENT_SCOPE)

set(TABGRAPH_PARSER_MIME_FUNCTION0 ${TABGRAPH_PARSER_MIME_FUNCTION0} "model/fbx" PARENT_SCOPE)
set(TABGRAPH_PARSER_MIME_FUNCTION1 ${TABGRAPH_PARSER_MIME_FUNCTION1} "ParseFBX" PARENT_SCOPE)
