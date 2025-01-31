Fetch_GLM()

set(MSG_CAMERA_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Camera/Frustum.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Camera/Projection.hpp)

add_library(MSG-Camera INTERFACE ${MSG_CAMERA_HEADER_FILES})
add_library(MSG::Camera ALIAS MSG-Camera)
target_include_directories(MSG-Camera INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Camera INTERFACE
  MSG::Shapes
  MSG::Transform)

set_target_properties(
  MSG-Camera
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
