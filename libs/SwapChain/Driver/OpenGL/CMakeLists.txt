cmake_policy(SET CMP0072 NEW)

Fetch_GLEW()

set(MSG_SWAPCHAIN_LIBS ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/SwapChain/OGL)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/SwapChain/OGL)

set(MSG_SWAPCHAIN_IMPL_HEADER_FILES
  ${MSG_SWAPCHAIN_IMPL_HEADER_FILES})

set(MSG_SWAPCHAIN_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/SwapChain.cpp)

if (WIN32)
  set(MSG_SWAPCHAIN_IMPL_HEADER_FILES ${MSG_SWAPCHAIN_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Win32/SwapChain.hpp)
  set(MSG_SWAPCHAIN_IMPL_SOURCE_FILES ${MSG_SWAPCHAIN_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Win32/SwapChain.cpp)
elseif(UNIX)
  find_package(X11)
  set(MSG_SWAPCHAIN_LIBS ${MSG_SWAPCHAIN_LIBS}
    ${X11_LIBRARIES})
  set(MSG_SWAPCHAIN_IMPL_HEADER_FILES ${MSG_SWAPCHAIN_IMPL_HEADER_FILES}
    ${INCLUDE_DIR}/Unix/Context.hpp
    ${INCLUDE_DIR}/Unix/SwapChain.hpp)
  set(MSG_SWAPCHAIN_IMPL_SOURCE_FILES ${MSG_SWAPCHAIN_IMPL_SOURCE_FILES}
    ${SOURCE_DIR}/Unix/Context.cpp
    ${SOURCE_DIR}/Unix/SwapChain.cpp)
endif(WIN32)

add_library(MSG-SwapChain-Implementation STATIC
  ${MSG_SWAPCHAIN_IMPL_SOURCE_FILES}
  ${MSG_SWAPCHAIN_IMPL_HEADER_FILES})
add_library(MSG::SwapChain::Implementation ALIAS MSG-SwapChain-Implementation)
target_compile_definitions(MSG-SwapChain-Implementation PUBLIC GLEW_NO_GLU)
target_compile_definitions(MSG-SwapChain-Implementation PUBLIC GLEW_STATIC)
target_include_directories(MSG-SwapChain-Implementation PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-SwapChain-Implementation
  MSG::SwapChain::Interface
  MSG::Renderer::Interface
  MSG::Renderer::Implementation
  MSG::Renderer::Shaders
  MSG::Renderer::ShadersLibrary
  ${MSG_SWAPCHAIN_LIBS})

set_target_properties(
  MSG-SwapChain-Implementation
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
