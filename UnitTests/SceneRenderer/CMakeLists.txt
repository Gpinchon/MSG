cmake_minimum_required(VERSION 3.19)
cmake_policy(SET CMP0074 NEW)

set(TABGRAPH_TEST_SG_SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_executable(TabGraph-UnitTests-SceneRenderer
  ${TABGRAPH_TEST_SG_SRC})
target_include_directories(TabGraph-UnitTests-SceneRenderer PRIVATE
  ${sdl_SOURCE_DIR}/include
)
target_link_libraries(TabGraph-UnitTests-SceneRenderer
  TabGraph::SG
  TabGraph::Renderer::Interface
  TabGraph::Renderer::Implementation
  TabGraph::Window::Interface
  TabGraph::Window::Implementation
  TabGraph::Keyboard::Interface
  TabGraph::Keyboard::Implementation
  TabGraph::Mouse::Interface
  TabGraph::Mouse::Implementation
  TabGraph::Assets::Parsers)

if(MSVC_IDE)
  set_property(TARGET TabGraph-UnitTests-SceneRenderer PROPERTY
    VS_DEBUGGER_ENVIRONMENT "${OCRA_DEBUGGER_ENV}")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
