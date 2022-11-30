set(TABGRAPH_ASSETS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Asset.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Parser.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Assets/Uri.hpp)

set(TABGRAPH_ASSETS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Asset.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Assets/Uri.cpp)

set(TABGRAPH_PARSER_HEADER_FILES CACHE INTERNAL "")
set(TABGRAPH_PARSER_SOURCE_FILES CACHE INTERNAL "")

add_library(TabGraph-Assets ${TABGRAPH_ASSETS_HEADER_FILES} ${TABGRAPH_ASSETS_SOURCE_FILES} ${TABGRAPH_PARSER_HEADER_FILES} ${TABGRAPH_PARSER_SOURCE_FILES})
add_library(TabGraph::Assets ALIAS TabGraph-Assets)
target_include_directories(TabGraph-Assets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Assets TabGraph::Tools TabGraph::SG)

add_subdirectory(Parsers)


set_target_properties(
  TabGraph-Assets
  PROPERTIES FOLDER "TabGraph/Assets"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
