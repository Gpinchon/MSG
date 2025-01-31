cmake_policy(SET CMP0072 NEW)

Fetch_GLEW()

set(MSG_SWAPCHAIN_LIBS ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SwapChain/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/SwapChain/OGL)

set(MSG_SWAPCHAIN_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/SwapChain.hpp)

set(MSG_SWAPCHAIN_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/SwapChain.cpp)

if(UNIX)
  find_package(X11)
  set(MSG_SWAPCHAIN_LIBS ${MSG_SWAPCHAIN_LIBS} ${X11_LIBRARIES})
endif(UNIX)

add_library(MSG-SwapChain-Implementation STATIC
  ${MSG_SWAPCHAIN_IMPL_SOURCE_FILES}
  ${MSG_SWAPCHAIN_IMPL_HEADER_FILES})
add_library(MSG::SwapChain::Implementation ALIAS MSG-SwapChain-Implementation)
target_compile_definitions(MSG-SwapChain-Implementation PUBLIC GLEW_NO_GLU)
target_compile_definitions(MSG-SwapChain-Implementation PUBLIC GLEW_STATIC)
target_include_directories(MSG-SwapChain-Implementation PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-SwapChain-Implementation
  MSG::SwapChain::Interface
  MSG::OGLContext
  MSG::OGLBuffer
  MSG::Renderer::Interface
  MSG::Renderer::Implementation
  MSG::Renderer::Shaders
  MSG::Renderer::ShadersLibrary
  ${MSG_SWAPCHAIN_LIBS})

set_target_properties(
  MSG-SwapChain-Implementation
  PROPERTIES FOLDER "MSG/SwapChain"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
