set(TABGRAPH_WINDOW_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Window/Events.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Window/Handles.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Window/Structs.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Window/Window.hpp
)

add_library(TabGraph-Window-Interface INTERFACE ${TABGRAPH_WINDOW_HEADER_FILES})
add_library(TabGraph::Window::Interface ALIAS TabGraph-Window-Interface)
target_include_directories(TabGraph-Window-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Window-Interface INTERFACE
  TabGraph::Renderer::Interface
  TabGraph::Events::Interface
  TabGraph::Tools)
set_target_properties(
  TabGraph-Window-Interface
  PROPERTIES FOLDER "TabGraph")

set(TABGRAPH_WINDOW_IMPL "SDL2" CACHE STRING
  "Window Impl used at compile-time")
set(TABGRAPH_WINDOW_IMPL_VALUES
  "SDL2;")
set_property(CACHE TABGRAPH_WINDOW_IMPL PROPERTY STRINGS ${TABGRAPH_WINDOW_IMPL})

add_compile_definitions(
  TABGRAPH_WINDOW_SDL2=0
  TABGRAPH_WINDOW_IMPL=TABGRAPH_WINDOW_${TABGRAPH_WINDOW_IMPL})

if (TABGRAPH_WINDOW_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(TABGRAPH_WINDOW_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(include)
endif()
