cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

Fetch_GoogleTest()

set(MSG_TEST_ECS_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(MSG-UnitTests-ECS
  ${MSG_TEST_ECS_SRC})
target_link_libraries(MSG-UnitTests-ECS PRIVATE
  gtest
  gtest_main
  MSG::SG)

add_test(MSG-UnitTests-ECS MSG-UnitTests)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
