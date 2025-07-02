cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(MSG_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(MSG-UnitTests-SceneRenderer
  ${MSG_TEST_SG_SRC})
target_include_directories(MSG-UnitTests-SceneRenderer PRIVATE
  ${sdl_SOURCE_DIR}/include
)
target_link_libraries(MSG-UnitTests-SceneRenderer
  MSG::ImageUtils
  MSG::Scene
  MSG::ShapeGenerator
  MSG::Renderer
  MSG::Window
  MSG::Keyboard
  MSG::Mouse
  MSG::Assets::Parsers)

if(MSVC_IDE)
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
