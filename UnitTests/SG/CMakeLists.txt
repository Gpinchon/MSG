cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

Fetch_GoogleTest()

set(TABGRAPH_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(TabGraph-UnitTests-SG
  ${TABGRAPH_TEST_SG_SRC})
target_link_libraries(TabGraph-UnitTests-SG PRIVATE
  gtest
  gtest_main
  TabGraph::SG)

add_test(TabGraph-UnitTests-SG TabGraph-UnitTests)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
