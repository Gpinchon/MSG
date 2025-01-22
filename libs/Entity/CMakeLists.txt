set(MSG_ENTITY_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Entity/PunctualLight.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Entity/Sky.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Entity/Node.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Entity/NodeGroup.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Entity/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Entity.hpp)

set(MSG_ENTITY_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Entity/PunctualLight.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Entity/Sky.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Entity/Node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Entity/NodeGroup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Entity/Camera.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Entity.cpp)

add_library(MSG-Entity ${MSG_ENTITY_SOURCE_FILES} ${MSG_ENTITY_HEADER_FILES})
add_library(MSG::Entity ALIAS MSG-Entity)
target_include_directories(MSG-Entity PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Entity
  MSG::Light
  MSG::Mesh)

set_target_properties(
  MSG-Entity
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
