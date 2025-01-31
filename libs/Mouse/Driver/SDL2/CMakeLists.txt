set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Mouse/SDL2)

set(MSG_MOUSE_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/EventButtonPressed.cpp
  ${SOURCE_DIR}/EventButtonReleased.cpp
  ${SOURCE_DIR}/EventMotion.cpp
  ${SOURCE_DIR}/EventWheel.cpp
  ${SOURCE_DIR}/Mouse.cpp
)

add_library(MSG-Mouse-Implementation STATIC
  ${MSG_MOUSE_IMPL_SOURCE_FILES})
add_library(MSG::Mouse::Implementation ALIAS MSG-Mouse-Implementation)
target_include_directories(MSG-Mouse-Implementation PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(MSG-Mouse-Implementation
  MSG::Mouse::Interface
  MSG::Events::Implementation)

set_target_properties(
  MSG-Mouse-Implementation
  PROPERTIES FOLDER "MSG/Mouse"
)

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()