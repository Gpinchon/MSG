set(MSG_TRANSFORM_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Transform/Data.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Transform.hpp)

set(MSG_TRANSFORM_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Transform/Data.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Transform.cpp)

add_library(MSG-Transform STATIC ${MSG_TRANSFORM_HEADER_FILES} ${MSG_TRANSFORM_SOURCE_FILES})
add_library(MSG::Transform ALIAS MSG-Transform)
target_include_directories(MSG-Transform PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-Transform PUBLIC MSG::Core)

set_target_properties(
  MSG-Transform
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
