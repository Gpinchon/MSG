set(MSG_WINDOW_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Window/Events.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Window/Handles.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Window/Structs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Window/Window.hpp
)

add_library(MSG-Window-Interface INTERFACE ${MSG_WINDOW_HEADER_FILES})
add_library(MSG::Window::Interface ALIAS MSG-Window-Interface)
target_include_directories(MSG-Window-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Window-Interface INTERFACE
  MSG::Renderer::Interface
  MSG::SwapChain::Interface
  MSG::Events::Interface
  MSG::Tools)
set_target_properties(
  MSG-Window-Interface
  PROPERTIES FOLDER "MSG")

set(MSG_WINDOW_IMPL "SDL2" CACHE STRING
  "Window Impl used at compile-time")
set(MSG_WINDOW_IMPL_VALUES
  "SDL2;")
set_property(CACHE MSG_WINDOW_IMPL PROPERTY STRINGS ${MSG_WINDOW_IMPL})

add_compile_definitions(
  MSG_WINDOW_SDL2=0
  MSG_WINDOW_IMPL=MSG_WINDOW_${MSG_WINDOW_IMPL})

if (MSG_WINDOW_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_WINDOW_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(include)
endif()
