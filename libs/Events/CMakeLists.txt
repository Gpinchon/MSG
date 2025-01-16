set(TABGRAPH_EVENTS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/Event.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/EventManager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/RegisteredEvent.hpp)

set(TABGRAPH_EVENTS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/EventManager.cpp)

add_library(TabGraph-Events-Interface INTERFACE ${TABGRAPH_EVENTS_HEADER_FILES})
add_library(TabGraph::Events::Interface ALIAS TabGraph-Events-Interface)
target_include_directories(TabGraph-Events-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Events-Interface INTERFACE
  TabGraph::Tools)

set_target_properties(
  TabGraph-Events-Interface
  PROPERTIES FOLDER "TabGraph")

set(TABGRAPH_EVENTS_IMPL "SDL2" CACHE STRING
  "Window Impl used at compile-time")
set(TABGRAPH_EVENTS_IMPL_VALUES
  "SDL2;")
set_property(CACHE TABGRAPH_EVENTS_IMPL PROPERTY STRINGS ${TABGRAPH_EVENTS_IMPL})

add_compile_definitions(
  TABGRAPH_EVENTS_SDL2=0
  TABGRAPH_EVENTS_IMPL=TABGRAPH_EVENTS_${TABGRAPH_EVENTS_IMPL})

if (TABGRAPH_EVENTS_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(TABGRAPH_EVENTS_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
