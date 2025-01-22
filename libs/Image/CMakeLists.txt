set(MSG_IMAGE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Cubemap.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Image1D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Image2D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Image3D.hpp)

set(MSG_IMAGE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Cubemap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Image1D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Image2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Image3D.cpp)

add_library(MSG-Image ${MSG_IMAGE_SOURCE_FILES} ${MSG_IMAGE_HEADER_FILES})
add_library(MSG::Image ALIAS MSG-Image)
target_include_directories(MSG-Image PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(MSG-Image
  MSG::Buffer
  MSG::PixelDescriptor)

set_target_properties(
  MSG-Image
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
