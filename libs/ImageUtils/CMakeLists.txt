set(MSG_IMAGE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ImageUtils.hpp)

set(MSG_IMAGE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/ImageUtils.cpp)

add_library(MSG-ImageUtils ${MSG_IMAGE_SOURCE_FILES} ${MSG_IMAGE_HEADER_FILES})
add_library(MSG::ImageUtils ALIAS MSG-ImageUtils)
target_include_directories(MSG-ImageUtils PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(MSG-ImageUtils
  MSG::Image
  MSG::Texture)

set_target_properties(
  MSG-ImageUtils
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
