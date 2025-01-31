Fetch_GLM()

set(MSG_BOUNDING_VOLUME_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/BoundingVolume.hpp)

add_library(MSG-BoundingVolume INTERFACE ${MSG_BOUNDING_VOLUME_HEADER_FILES})
add_library(MSG::BoundingVolume ALIAS MSG-BoundingVolume)
target_include_directories(MSG-BoundingVolume INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-BoundingVolume INTERFACE MSG::Shapes)

set_target_properties(
  MSG-BoundingVolume
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
