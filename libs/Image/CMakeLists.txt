set(MSG_IMAGE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Image/Clamp.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Image/ManhattanRound.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cubemap.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Image.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Image1D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Image2D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Image3D.hpp)

set(MSG_IMAGE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cubemap.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Image.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Image1D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Image2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Image3D.cpp)

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
  GroupSources(src)
  GroupSources(include)
endif()
