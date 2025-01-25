Fetch_SDL2()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Window/SDL2)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Window/SDL2)

set(MSG_WINDOW_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/Window.hpp
)

set(MSG_WINDOW_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Events.cpp
  ${SOURCE_DIR}/Window.cpp
)

add_library(MSG-Window-Implementation STATIC
  ${MSG_WINDOW_IMPL_HEADER_FILES}
  ${MSG_WINDOW_IMPL_SOURCE_FILES})
add_library(MSG::Window::Implementation ALIAS MSG-Window-Implementation)
target_include_directories(MSG-Window-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(MSG-Window-Implementation
  MSG::Window::Interface
  MSG::OGLContext::Implementation
  MSG::SwapChain::Implementation
  MSG::Renderer::Implementation
  MSG::Events::Implementation
  MSG::Tools
  SDL2::SDL2-static
)

set_target_properties(
  MSG-Window-Implementation
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()