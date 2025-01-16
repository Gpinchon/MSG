Fetch_SDL2()

set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Mouse/SDL2)

set(TABGRAPH_MOUSE_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/EventButtonPressed.cpp
  ${SOURCE_DIR}/EventButtonReleased.cpp
  ${SOURCE_DIR}/EventMotion.cpp
  ${SOURCE_DIR}/EventWheel.cpp
  ${SOURCE_DIR}/Mouse.cpp
)

add_library(TabGraph-Mouse-Implementation STATIC
  ${TABGRAPH_MOUSE_IMPL_SOURCE_FILES})
add_library(TabGraph::Mouse::Implementation ALIAS TabGraph-Mouse-Implementation)
target_include_directories(TabGraph-Mouse-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(TabGraph-Mouse-Implementation
  TabGraph::Mouse::Interface
  TabGraph::Events::Implementation
  TabGraph::Tools
  SDL2::SDL2-static
)

set_target_properties(
  TabGraph-Mouse-Implementation
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()