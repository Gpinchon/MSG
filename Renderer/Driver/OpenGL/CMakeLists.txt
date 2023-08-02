include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

Fetch_GLEW()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/OGL)

set(TABGRAPH_RENDERER_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/Win32/Error.hpp
  ${INCLUDE_DIR}/Win32/OpenGL.hpp
  ${INCLUDE_DIR}/Win32/Window.hpp
  ${INCLUDE_DIR}/Renderer.hpp
)

set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Win32/Error.cpp
  ${SOURCE_DIR}/Win32/OpenGL.cpp
  ${SOURCE_DIR}/Win32/Window.cpp
  ${SOURCE_DIR}/Renderer.cpp
)

add_library(TabGraph-Renderer-Implementation ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES} ${TABGRAPH_RENDERER_IMPL_HEADER_FILES})
add_library(TabGraph::Renderer::Implementation ALIAS TabGraph-Renderer-Implementation)
target_include_directories(TabGraph-Renderer-Implementation PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Renderer-Implementation
  TabGraph::SG
  TabGraph::Renderer::Interface
  GLEW::GLEW)

set_target_properties(
  TabGraph-Renderer-Implementation
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
