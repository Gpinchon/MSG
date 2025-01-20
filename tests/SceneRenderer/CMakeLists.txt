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
  MSG::SG
  MSG::Renderer::Interface
  MSG::Renderer::Implementation
  MSG::Window::Interface
  MSG::Window::Implementation
  MSG::Keyboard::Interface
  MSG::Keyboard::Implementation
  MSG::Mouse::Interface
  MSG::Mouse::Implementation
  MSG::Assets::Parsers)

if(MSVC_IDE)
  set_property(TARGET MSG-UnitTests-SceneRenderer PROPERTY
    VS_DEBUGGER_ENVIRONMENT "${OCRA_DEBUGGER_ENV}")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
