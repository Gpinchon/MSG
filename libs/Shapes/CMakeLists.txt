Fetch_GLM()

set(MSG_SHAPES_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Shape.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cube.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Plane.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Sphere.hpp)

set(MSG_SHAPES_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Shape.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cube.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Plane.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Sphere.cpp)

add_library(MSG-Shapes STATIC ${MSG_SHAPES_HEADER_FILES} ${MSG_SHAPES_SOURCE_FILES})
add_library(MSG::Shapes ALIAS MSG-Shapes)
target_include_directories(MSG-Shapes PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Shapes PUBLIC glm::glm)

set_target_properties(
  MSG-Shapes
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
