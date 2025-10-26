cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

Fetch_GoogleTest()

set(MSG_TEST_SH_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(MSG-UnitTests-SH
  ${MSG_TEST_SH_SRC})
target_link_libraries(MSG-UnitTests-SH PRIVATE
  gtest
  gtest_main
  MSG::SH
  MSG::Tools)

add_test(MSG-UnitTests-SH MSG-UnitTests)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
