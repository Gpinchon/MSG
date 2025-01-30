set(MSG_KEYBOARD_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Keyboard/Enums.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Keyboard/Scancode.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Keyboard/Keycode.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Keyboard/Events.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Keyboard/Keyboard.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Keyboard/Structs.hpp
)

add_library(MSG-Keyboard-Interface INTERFACE ${MSG_KEYBOARD_HEADER_FILES})
add_library(MSG::Keyboard::Interface ALIAS MSG-Keyboard-Interface)
target_include_directories(MSG-Keyboard-Interface PUBLIC INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Keyboard-Interface INTERFACE
  MSG::Events::Interface
  MSG::Tools)
set_target_properties(
  MSG-Keyboard-Interface
  PROPERTIES FOLDER "MSG/Keyboard")

set(MSG_KEYBOARD_IMPL ${MSG_EVENTS_IMPL} CACHE STRING
  "Keyboard Impl used at compile-time")
set(MSG_KEYBOARD_IMPL_VALUES
  "SDL2;")
set_property(CACHE MSG_KEYBOARD_IMPL PROPERTY STRINGS ${MSG_KEYBOARD_IMPL})

add_compile_definitions(
  MSG_KEYBOARD_SDL2=0
  MSG_KEYBOARD_IMPL=MSG_KEYBOARD_${MSG_KEYBOARD_IMPL})

if (MSG_KEYBOARD_IMPL STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_KEYBOARD_IMPL STREQUAL "SDL2")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(include)
endif()
