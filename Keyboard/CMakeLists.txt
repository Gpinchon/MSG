set(TABGRAPH_KEYBOARD_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Keyboard/Enums.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Keyboard/Events.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Keyboard/Keyboard.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Keyboard/Structs.hpp
)

add_library(TabGraph-Keyboard-Interface INTERFACE ${TABGRAPH_KEYBOARD_HEADER_FILES})
add_library(TabGraph::Keyboard::Interface ALIAS TabGraph-Keyboard-Interface)
target_include_directories(TabGraph-Keyboard-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Keyboard-Interface INTERFACE
  TabGraph::Events::Interface
  TabGraph::Tools)
set_target_properties(
  TabGraph-Keyboard-Interface
  PROPERTIES FOLDER "TabGraph")

set(TABGRAPH_KEYBOARD_IMPL "SDL2" CACHE STRING
  "Keyboard Impl used at compile-time")
set(TABGRAPH_WINDOW_IMPL_VALUES
  "SDL2;")
set_property(CACHE TABGRAPH_KEYBOARD_IMPL PROPERTY STRINGS ${TABGRAPH_KEYBOARD_IMPL})

add_compile_definitions(
  TABGRAPH_KEYBOARD_SDL2=0
  TABGRAPH_KEYBOARD_IMPL=TABGRAPH_KEYBOARD_${TABGRAPH_KEYBOARD_IMPL})

if (TABGRAPH_KEYBOARD_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(TABGRAPH_KEYBOARD_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(include)
endif()
