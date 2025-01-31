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
  MSG::Events::Interface)

set_target_properties(
  MSG-Mouse-Interface
  PROPERTIES FOLDER "MSG/Mouse")

if (MSG_EVENTS_BACKEND STREQUAL "SDL2")
  add_subdirectory(Driver/SDL2)
endif(MSG_EVENTS_BACKEND STREQUAL "SDL2")

add_library(MSG-Mouse INTERFACE)
add_library(MSG::Mouse ALIAS MSG-Mouse)
target_link_libraries(MSG-Mouse INTERFACE
    MSG::Mouse::Interface
    MSG::Mouse::Implementation)

if(MSVC_IDE)
  GroupSources(include)
endif()
