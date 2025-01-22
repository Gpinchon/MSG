Fetch_GLM()

set(MSG_CORE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Shapes/Box.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Shapes/Plane.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Shapes/Sphere.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/BoundingVolume.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Camera.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Children.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/DataType.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Frustum.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Inherit.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Name.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Object.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Orientation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Parent.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Projection.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Property.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Core/Transform.hpp)

set(MSG_CORE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Object.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Core/Orientation.cpp)

add_library(MSG-Core ${MSG_CORE_SOURCE_FILES} ${MSG_CORE_HEADER_FILES})
add_library(MSG::Core ALIAS MSG-Core)
target_include_directories(MSG-Core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Core MSG::ECS glm::glm)

set_target_properties(
  MSG-Core
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
