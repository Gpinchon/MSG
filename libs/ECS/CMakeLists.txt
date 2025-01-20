include(FetchContent)

set(MSG_ECS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/ComponentTypeStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityRef.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/EntityStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/Registry.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ECS/View.hpp)

add_library(MSG-ECS INTERFACE ${MSG_ECS_HEADER_FILES})
add_library(MSG::ECS ALIAS MSG-ECS)
target_include_directories(MSG-ECS INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-ECS PUBLIC INTERFACE
  MSG::Tools)

set_target_properties(
  MSG-ECS
  PROPERTIES FOLDER "MSG"
)

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
