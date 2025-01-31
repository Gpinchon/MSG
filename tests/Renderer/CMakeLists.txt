cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(MSG_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(MSG-UnitTests-Renderer
  ${MSG_TEST_SG_SRC})
target_link_libraries(MSG-UnitTests-Renderer
  MSG::Scene
  MSG::ShapeGenerator
  MSG::Renderer
  MSG::Window
  MSG::SwapChain
  MSG::Assets::Parsers)

if(MSVC_IDE)
  set_property(TARGET MSG-UnitTests-Renderer PROPERTY
    VS_DEBUGGER_ENVIRONMENT "${OCRA_DEBUGGER_ENV}")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
