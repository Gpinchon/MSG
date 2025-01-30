set(MSG_MOUSE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Mouse/Enums.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Mouse/Events.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Mouse/Mouse.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Mouse/Structs.hpp
)

add_library(MSG-Mouse-Interface INTERFACE ${MSG_MOUSE_HEADER_FILES})
add_library(MSG::Mouse::Interface ALIAS MSG-Mouse-Interface)
target_include_directories(MSG-Mouse-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Mouse-Interface INTERFACE
  MSG::Events::Interface
  MSG::Tools)
set_target_properties(
  MSG-Mouse-Interface
  PROPERTIES FOLDER "MSG/Mouse")

set(MSG_MOUSE_IMPL ${MSG_EVENTS_IMPL} CACHE STRING
  "Mouse Impl used at compile-time")
set(MSG_MOUSE_IMPL_VALUES
  "SDL2;")
set_property(CACHE MSG_MOUSE_IMPL PROPERTY STRINGS ${MSG_MOUSE_IMPL})

add_compile_definitions(
  MSG_MOUSE_SDL2=0
  MSG_MOUSE_IMPL=MSG_MOUSE_${MSG_MOUSE_IMPL})

if (MSG_MOUSE_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_MOUSE_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(include)
endif()
