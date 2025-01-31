set(MSG_EVENTS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Events/Event.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Events/EventManager.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Events/RegisteredEvent.hpp)

add_library(MSG-Events-Interface INTERFACE ${MSG_EVENTS_HEADER_FILES})
add_library(MSG::Events::Interface ALIAS MSG-Events-Interface)
target_include_directories(MSG-Events-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Events-Interface INTERFACE
  MSG::Tools)

set_target_properties(
  MSG-Events-Interface
  PROPERTIES FOLDER "MSG/Events")

if (MSG_EVENTS_BACKEND STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_EVENTS_BACKEND STREQUAL "SDL2")

add_library(MSG-Events INTERFACE)
add_library(MSG::Events ALIAS MSG-Events)
target_link_libraries(MSG-Events INTERFACE
    MSG::Events::Interface
    MSG::Events::Implementation)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
