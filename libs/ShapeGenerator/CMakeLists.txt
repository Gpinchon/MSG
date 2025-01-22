set(MSG_SHAPE_GENERATOR_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ShapeGenerator/Capsule.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ShapeGenerator/Cube.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ShapeGenerator/Plane.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/ShapeGenerator/Sphere.hpp)

set(MSG_SHAPE_GENERATOR_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShapeGenerator/Capsule.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShapeGenerator/Cube.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShapeGenerator/Plane.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ShapeGenerator/Sphere.cpp)

add_library(MSG-ShapeGenerator ${MSG_SHAPE_GENERATOR_SOURCE_FILES} ${MSG_SHAPE_GENERATOR_HEADER_FILES})
add_library(MSG::ShapeGenerator ALIAS MSG-ShapeGenerator)
target_include_directories(MSG-ShapeGenerator PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-ShapeGenerator MSG::Mesh)

set_target_properties(
  MSG-ShapeGenerator
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
