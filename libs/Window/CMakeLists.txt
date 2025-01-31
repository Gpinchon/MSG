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
  MSG::Events::Interface)

set_target_properties(
  MSG-Window-Interface
  PROPERTIES FOLDER "MSG/Window")

if (MSG_EVENTS_BACKEND STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_EVENTS_BACKEND STREQUAL "SDL2")

add_library(MSG-Window INTERFACE)
add_library(MSG::Window ALIAS MSG-Window)
target_link_libraries(MSG-Window INTERFACE
    MSG::Window::Interface
    MSG::Window::Implementation)

if(MSVC_IDE)
  # Run macro
  GroupSources(include)
endif()
