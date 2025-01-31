set(MSG_TRANSFORM_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Transform/Data.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Transform.hpp)

add_library(MSG-Transform INTERFACE ${MSG_TRANSFORM_HEADER_FILES})
add_library(MSG::Transform ALIAS MSG-Transform)
target_include_directories(MSG-Transform INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Transform INTERFACE MSG::Core)

set_target_properties(
  MSG-Transform
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
