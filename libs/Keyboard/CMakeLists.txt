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
  MSG::Events::Interface)

set_target_properties(
  MSG-Keyboard-Interface
  PROPERTIES FOLDER "MSG/Keyboard")

if (MSG_EVENTS_BACKEND STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_EVENTS_BACKEND STREQUAL "SDL2")

add_library(MSG-Keyboard INTERFACE)
add_library(MSG::Keyboard ALIAS MSG-Keyboard)
target_link_libraries(MSG-Keyboard INTERFACE
    MSG::Keyboard::Interface
    MSG::Keyboard::Implementation)

if(MSVC_IDE)
  GroupSources(include)
endif()
