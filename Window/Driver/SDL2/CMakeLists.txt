Fetch_SDL2()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Window/SDL2)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Window/SDL2)

set(TABGRAPH_WINDOW_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/Window.hpp
)

set(TABGRAPH_WINDOW_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/Events.cpp
  ${SOURCE_DIR}/Window.cpp
)

add_library(TabGraph-Window-Implementation STATIC
  ${TABGRAPH_WINDOW_IMPL_HEADER_FILES}
  ${TABGRAPH_WINDOW_IMPL_SOURCE_FILES})
add_library(TabGraph::Window::Implementation ALIAS TabGraph-Window-Implementation)
target_include_directories(TabGraph-Window-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(TabGraph-Window-Implementation
  TabGraph::Window::Interface
  TabGraph::Events::Implementation
  TabGraph::Tools
  SDL2::SDL2main
  SDL2::SDL2-static
)

set_target_properties(
  TabGraph-Window-Implementation
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()