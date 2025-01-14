cmake_policy(SET CMP0072 NEW)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Fetch_Dependencies.cmake)

#Fetch_GLEW()

set(TABGRAPH_SWAPCHAIN_LIBS
  GLEW::GLEW)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/SwapChain/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/SwapChain/OGL)

set(TABGRAPH_SWAPCHAIN_IMPL_HEADER_FILES
  ${TABGRAPH_SWAPCHAIN_IMPL_HEADER_FILES}
  )

set(TABGRAPH_SWAPCHAIN_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/SwapChain.cpp)

if (WIN32)
  set(TABGRAPH_SWAPCHAIN_IMPL_HEADER_FILES ${TABGRAPH_SWAPCHAIN_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Win32/SwapChain.hpp)

  set(TABGRAPH_SWAPCHAIN_IMPL_SOURCE_FILES ${TABGRAPH_SWAPCHAIN_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Win32/SwapChain.cpp)
elseif(UNIX)
  find_package(X11)
  set(TABGRAPH_SWAPCHAIN_LIBS ${TABGRAPH_SWAPCHAIN_LIBS}
    ${X11_LIBRARIES})
  find_package(OpenGL)
  set(TABGRAPH_SWAPCHAIN_LIBS ${TABGRAPH_SWAPCHAIN_LIBS}
    OpenGL::GL OpenGL::EGL)
  message(${TABGRAPH_SWAPCHAIN_LIBS})

  set(TABGRAPH_SWAPCHAIN_IMPL_HEADER_FILES ${TABGRAPH_SWAPCHAIN_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Unix/Context.hpp
    ${INCLUDE_DIR}/Unix/SwapChain.hpp)

  set(TABGRAPH_SWAPCHAIN_IMPL_SOURCE_FILES ${TABGRAPH_SWAPCHAIN_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Unix/Context.cpp
    ${SOURCE_DIR}/Unix/SwapChain.cpp)
endif(WIN32)

add_library(TabGraph-SwapChain-Implementation STATIC
  ${TABGRAPH_SWAPCHAIN_IMPL_SOURCE_FILES}
  ${TABGRAPH_SWAPCHAIN_IMPL_HEADER_FILES})
add_library(TabGraph::SwapChain::Implementation ALIAS TabGraph-SwapChain-Implementation)
target_compile_definitions(TabGraph-SwapChain-Implementation PUBLIC GLEW_NO_GLU)
target_compile_definitions(TabGraph-SwapChain-Implementation PUBLIC GLEW_STATIC)
target_include_directories(TabGraph-SwapChain-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-SwapChain-Implementation
  TabGraph::SwapChain::Interface
  TabGraph::Renderer::Interface
  TabGraph::Renderer::Implementation
  TabGraph::Renderer::Shaders
  TabGraph::Renderer::ShadersLibrary
  ${TABGRAPH_SWAPCHAIN_LIBS})

set_target_properties(
  TabGraph-SwapChain-Implementation
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
