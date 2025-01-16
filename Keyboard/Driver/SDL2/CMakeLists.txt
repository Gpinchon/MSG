Fetch_SDL2()

set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Keyboard/SDL2)

set(TABGRAPH_KEYBOARD_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/EventKeyPressed.cpp
  ${SOURCE_DIR}/EventKeyReleased.cpp
  ${SOURCE_DIR}/Keyboard.cpp
)

add_library(TabGraph-Keyboard-Implementation STATIC
  ${TABGRAPH_KEYBOARD_IMPL_SOURCE_FILES})
add_library(TabGraph::Keyboard::Implementation ALIAS TabGraph-Keyboard-Implementation)
target_include_directories(TabGraph-Keyboard-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(TabGraph-Keyboard-Implementation
  TabGraph::Keyboard::Interface
  TabGraph::Events::Implementation
  TabGraph::Tools
  SDL2::SDL2-static
)

set_target_properties(
  TabGraph-Keyboard-Implementation
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()