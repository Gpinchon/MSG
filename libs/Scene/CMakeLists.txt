set(MSG_SCENE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/Animation/Channel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/Animation/Interpolator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/Animation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/CullResult.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene/Octree.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Scene.hpp)

set(MSG_SCENE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene/Animation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Scene.cpp)

add_library(MSG-Scene ${MSG_SCENE_SOURCE_FILES} ${MSG_SCENE_HEADER_FILES})
add_library(MSG::Scene ALIAS MSG-Scene)
target_include_directories(MSG-Scene PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Scene
  MSG::Core
  MSG::ECS
  MSG::Entity)

set_target_properties(
  MSG-Scene
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
