set(MSG_SCENE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Scene/CullResult.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Scene/Octree.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Scene.hpp)

set(MSG_SCENE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Scene.cpp)

add_library(MSG-Scene ${MSG_SCENE_SOURCE_FILES} ${MSG_SCENE_HEADER_FILES})
add_library(MSG::Scene ALIAS MSG-Scene)
target_include_directories(MSG-Scene PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Scene
  MSG::Core
  MSG::Light
  MSG::Mesh
  MSG::Entity
  MSG::Animation)

set_target_properties(
  MSG-Scene
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
