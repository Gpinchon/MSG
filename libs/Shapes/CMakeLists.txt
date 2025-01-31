Fetch_GLM()

set(MSG_SHAPES_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Box.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Plane.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Sphere.hpp)

add_library(MSG-Shapes INTERFACE ${MSG_SHAPES_HEADER_FILES})
add_library(MSG::Shapes ALIAS MSG-Shapes)
target_include_directories(MSG-Shapes INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Shapes INTERFACE glm::glm)

set_target_properties(
  MSG-Shapes
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
