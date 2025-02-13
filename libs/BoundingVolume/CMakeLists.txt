Fetch_GLM()

set(MSG_BOUNDING_VOLUME_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/BoundingVolume.hpp)

set(MSG_BOUNDING_VOLUME_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/BoundingVolume.cpp)

add_library(MSG-BoundingVolume STATIC ${MSG_BOUNDING_VOLUME_HEADER_FILES} ${MSG_BOUNDING_VOLUME_SOURCE_FILES})
add_library(MSG::BoundingVolume ALIAS MSG-BoundingVolume)
target_include_directories(MSG-BoundingVolume PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-BoundingVolume PUBLIC MSG::Shapes)

set_target_properties(
  MSG-BoundingVolume
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
