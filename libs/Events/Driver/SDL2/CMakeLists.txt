Fetch_SDL2()

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/Events/SDL2)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/Events/SDL2)

set(MSG_EVENTS_IMPL_HEADER_FILES
  ${INCLUDE_DIR}/EventManager.hpp
)

set(MSG_EVENTS_IMPL_SOURCE_FILES
  ${SOURCE_DIR}/EventManager.cpp
)

add_library(MSG-Events-Implementation STATIC
  ${MSG_EVENTS_IMPL_HEADER_FILES}
  ${MSG_EVENTS_IMPL_SOURCE_FILES})
add_library(MSG::Events::Implementation ALIAS MSG-Events-Implementation)
target_include_directories(MSG-Events-Implementation PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(MSG-Events-Implementation
  MSG::Events::Interface
  SDL2::SDL2main
  SDL2::SDL2-static
)

set_target_properties(
  MSG-Events-Implementation
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()