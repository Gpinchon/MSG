Fetch_SDL2()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/SDL2)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/SDL2)

set(TABGRAPH_EVENTS_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/EventManager.hpp
)

set(TABGRAPH_EVENTS_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/EventManager.cpp
)

add_library(TabGraph-Events-Implementation STATIC
  ${TABGRAPH_EVENTS_IMPL_HEADER_FILES}
  ${TABGRAPH_EVENTS_IMPL_SOURCE_FILES})
add_library(TabGraph::Events::Implementation ALIAS TabGraph-Events-Implementation)
target_include_directories(TabGraph-Events-Implementation PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(TabGraph-Events-Implementation
  TabGraph::Events::Interface
  SDL2::SDL2main
  SDL2::SDL2-static
)

set_target_properties(
  TabGraph-Events-Implementation
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()