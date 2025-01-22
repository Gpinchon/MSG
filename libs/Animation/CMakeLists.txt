set(MSG_ANIMATION_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/Channel.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Animation/Interpolator.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Animation.hpp)

set(MSG_ANIMATION_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Animation.cpp)

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
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
