set(MSG_MESH_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/MeshInstanceModifier.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/MeshInstances.hpp)

set(MSG_MESH_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/MeshInstances.cpp)

add_library(MSG-MeshInstances ${MSG_MESH_SOURCE_FILES} ${MSG_MESH_HEADER_FILES})
add_library(MSG::MeshInstances ALIAS MSG-MeshInstances)
target_include_directories(MSG-MeshInstances PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-MeshInstances
  MSG::Mesh)

set_target_properties(
  MSG-MeshInstances
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
