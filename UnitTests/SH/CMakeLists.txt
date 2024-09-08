cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

Fetch_GoogleTest()

set(TABGRAPH_TEST_SH_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(TabGraph-UnitTests-SH
  ${TABGRAPH_TEST_SH_SRC})
target_link_libraries(TabGraph-UnitTests-SH PRIVATE
  gtest
  gtest_main
  TabGraph::Tools)

add_test(TabGraph-UnitTests-SH TabGraph-UnitTests)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
