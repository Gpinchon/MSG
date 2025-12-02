Fetch_GLM()

set(MSG_CAMERA_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Camera/Settings.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Camera/Frustum.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Camera/Projection.hpp)

set(MSG_CAMERA_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Camera/Projection.cpp)

add_library(MSG-Camera STATIC ${MSG_CAMERA_HEADER_FILES} ${MSG_CAMERA_SOURCE_FILES})
add_library(MSG::Camera ALIAS MSG-Camera)
target_include_directories(MSG-Camera PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Camera PUBLIC
  MSG::Component  
  MSG::Shapes
  MSG::Transform)

set_target_properties(
  MSG-Camera
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
