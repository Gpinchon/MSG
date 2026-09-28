cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(MSG_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(MSG-UnitTests-SwapChain ${MSG_TEST_SG_SRC})
add_dependencies(MSG-UnitTests-SwapChain Vulkan-ValidationLayers)
target_link_libraries(MSG-UnitTests-SwapChain
  MSG::Window
  MSG::Renderer
  gtest
  gtest_main)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
    set_property(TARGET MSG-UnitTests-SwapChain APPEND_STRING PROPERTY
    VS_DEBUGGER_ENVIRONMENT "PATH=$(VC_ExecutablePath_x64);%PATH%\n${MSG_DEBUGGER_ENV}")
endif()
