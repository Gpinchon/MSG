set(TABGRAPH_EVENTS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/Event.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/EventManager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/RegisteredEvent.hpp)

set(TABGRAPH_EVENTS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/EventManager.cpp)

add_library(TabGraph-Events ${TABGRAPH_EVENTS_SOURCE_FILES} ${TABGRAPH_EVENTS_HEADER_FILES})
add_library(TabGraph::Events ALIAS TabGraph-Events)
target_include_directories(TabGraph-Events PUBLIC 
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(TabGraph-Events
  TabGraph::Tools)

set_target_properties(
  TabGraph-Events
  PROPERTIES FOLDER "TabGraph"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
