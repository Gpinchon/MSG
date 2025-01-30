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

set(MSG_EVENTS_IMPL "SDL2" CACHE STRING
  "Window Impl used at compile-time")
set(MSG_EVENTS_IMPL_VALUES
  "SDL2;")
set_property(CACHE MSG_EVENTS_IMPL PROPERTY STRINGS ${MSG_EVENTS_IMPL})

add_compile_definitions(
  MSG_EVENTS_SDL2=0
  MSG_EVENTS_IMPL=MSG_EVENTS_${MSG_EVENTS_IMPL})

if (MSG_EVENTS_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_EVENTS_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
