set(MSG_IMAGE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/ImageStorage.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Image.hpp)

set(MSG_IMAGE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/ImageStorage.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Image.cpp)

add_library(MSG-Image ${MSG_IMAGE_SOURCE_FILES} ${MSG_IMAGE_HEADER_FILES})
add_library(MSG::Image ALIAS MSG-Image)
target_include_directories(MSG-Image PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(MSG-Image
  MSG::PixelDescriptor
  MSG::Threads
  MSG::PageFile
  MSG::Debug)

set_target_properties(
  MSG-Image
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
