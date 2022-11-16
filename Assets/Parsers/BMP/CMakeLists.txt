cmake_minimum_required(VERSION 3.19)

set(TABGRAPH_BMP_DATA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/BMP.cpp)

add_library(TabGraph-Assets-Parsers-BMP ${TABGRAPH_BMP_DATA_SOURCE_FILES})
target_link_libraries(TabGraph-Assets-Parsers-BMP TabGraph::Assets)

set_target_properties(
  TabGraph-Assets-Parsers-BMP
  PROPERTIES FOLDER "TabGraph/Assets/Parsers"
)

set(TABGRAPH_PARSER_LIBS ${TABGRAPH_PARSER_LIBS} TabGraph-Assets-Parsers-BMP PARENT_SCOPE)
set(TABGRAPH_PARSER_FUNCTIONS ${TABGRAPH_PARSER_FUNCTIONS} "ParseBMP" PARENT_SCOPE)
