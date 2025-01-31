
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Keyboard/SDL2)

set(MSG_KEYBOARD_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/EventKeyPressed.cpp
  ${SOURCE_DIR}/EventKeyReleased.cpp
  ${SOURCE_DIR}/Keyboard.cpp
)

add_library(MSG-Keyboard-Implementation STATIC
  ${MSG_KEYBOARD_IMPL_SOURCE_FILES})
add_library(MSG::Keyboard::Implementation ALIAS MSG-Keyboard-Implementation)
target_include_directories(MSG-Keyboard-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(MSG-Keyboard-Implementation
  MSG::Keyboard::Interface
  MSG::Events::Implementation)

set_target_properties(
  MSG-Keyboard-Implementation
  PROPERTIES FOLDER "MSG/Keyboard")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()