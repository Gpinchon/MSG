include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

Fetch_GLEW()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Renderer/OGL)

set(TABGRAPH_RENDERER_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/RAII/DebugGroup.hpp
  ${INCLUDE_DIR}/RAII/FrameBuffer.hpp
  ${INCLUDE_DIR}/RAII/Sampler.hpp
  ${INCLUDE_DIR}/RAII/Texture.hpp
  ${INCLUDE_DIR}/RAII/Wrapper.hpp
  ${INCLUDE_DIR}/RenderBuffer.hpp
  ${INCLUDE_DIR}/Renderer.hpp
)

set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/RAII/DebugGroup.cpp
  ${SOURCE_DIR}/RAII/FrameBuffer.cpp
  ${SOURCE_DIR}/RAII/Sampler.cpp
  ${SOURCE_DIR}/RAII/Texture.cpp
  ${SOURCE_DIR}/RenderBuffer.cpp
  ${SOURCE_DIR}/Renderer.cpp
  ${SOURCE_DIR}/SwapChain.cpp
)

if (WIN32)
set(TABGRAPH_RENDERER_IMPL_HEADER_FILES ${TABGRAPH_RENDERER_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/Win32/Context.hpp
  ${INCLUDE_DIR}/Win32/Error.hpp
  ${INCLUDE_DIR}/Win32/SwapChain.hpp
  ${INCLUDE_DIR}/Win32/Window.hpp
)

set(TABGRAPH_RENDERER_IMPL_SOURCE_FILES ${TABGRAPH_RENDERER_IMPL_SOURCE_FILES}
  ${SOURCE_DIR}/Win32/Context.cpp
  ${SOURCE_DIR}/Win32/Error.cpp
  ${SOURCE_DIR}/Win32/SwapChain.cpp
  ${SOURCE_DIR}/Win32/Window.cpp
)
endif(WIN32)

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
