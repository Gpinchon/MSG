set(MSG_ANIMATION_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Animation/Channel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Animation/Interpolator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Animation.hpp)

set(MSG_ANIMATION_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Animation.cpp)

add_library(MSG-Animation ${MSG_ANIMATION_SOURCE_FILES} ${MSG_ANIMATION_HEADER_FILES})
add_library(MSG::Animation ALIAS MSG-Animation)
target_include_directories(MSG-Animation PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Animation
  MSG::Core
  MSG::ECS
  MSG::Entity)

set_target_properties(
  MSG-Animation
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
