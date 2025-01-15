set(TABGRAPH_MOUSE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Mouse/Enums.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Mouse/Events.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Mouse/Mouse.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Mouse/Structs.hpp
)

add_library(TabGraph-Mouse-Interface INTERFACE ${TABGRAPH_MOUSE_HEADER_FILES})
add_library(TabGraph::Mouse::Interface ALIAS TabGraph-Mouse-Interface)
target_include_directories(TabGraph-Mouse-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Mouse-Interface INTERFACE
  TabGraph::Events::Interface
  TabGraph::Tools)
set_target_properties(
  TabGraph-Mouse-Interface
  PROPERTIES FOLDER "TabGraph")

set(TABGRAPH_MOUSE_IMPL "SDL2" CACHE STRING
  "Mouse Impl used at compile-time")
set(TABGRAPH_MOUSE_IMPL_VALUES
  "SDL2;")
set_property(CACHE TABGRAPH_MOUSE_IMPL PROPERTY STRINGS ${TABGRAPH_MOUSE_IMPL})

add_compile_definitions(
  TABGRAPH_MOUSE_SDL2=0
  TABGRAPH_MOUSE_IMPL=TABGRAPH_MOUSE_${TABGRAPH_MOUSE_IMPL})

if (TABGRAPH_MOUSE_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(TABGRAPH_MOUSE_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(include)
endif()
