set(MSG_MESH_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Mesh/Skin.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Mesh/Primitive.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Mesh.hpp)

set(MSG_MESH_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Mesh/Primitive.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Mesh.cpp)

add_library(MSG-Mesh ${MSG_MESH_SOURCE_FILES} ${MSG_MESH_HEADER_FILES})
add_library(MSG::Mesh ALIAS MSG-Mesh)
target_include_directories(MSG-Mesh PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Mesh
  MSG::BoundingVolume
  MSG::Material)

set_target_properties(
  MSG-Mesh
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
