cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(TABGRAPH_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(TabGraph-UnitTests-Renderer
  ${TABGRAPH_TEST_SG_SRC})
target_link_libraries(TabGraph-UnitTests-Renderer
  TabGraph::SG
  TabGraph::Renderer::Interface
  TabGraph::Renderer::Implementation
  TabGraph::Assets::Parsers)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
