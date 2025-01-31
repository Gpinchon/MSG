Fetch_GLM()

set(MSG_CORE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Core/DataType.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Core/Inherit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Core/Name.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Core/Object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Core/Orientation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Core/Property.hpp)

set(MSG_CORE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Core/Object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Core/Orientation.cpp)

add_library(MSG-Core ${MSG_CORE_SOURCE_FILES} ${MSG_CORE_HEADER_FILES})
add_library(MSG::Core ALIAS MSG-Core)
target_include_directories(MSG-Core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Core
  MSG::ECS
  glm::glm)

set_target_properties(
  MSG-Core
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
