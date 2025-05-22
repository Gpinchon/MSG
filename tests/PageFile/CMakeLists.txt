cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

Fetch_GoogleTest()

set(MSG_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(MSG-UnitTests-PageFile
  ${MSG_TEST_SG_SRC})
target_link_libraries(MSG-UnitTests-PageFile
  gtest
  gtest_main
  MSG::PageFile)

add_test(PageFile MSG-UnitTests)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
