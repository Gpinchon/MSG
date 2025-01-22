set(MSG_PIXEL_DESCRIPTOR_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/PixelDescriptor.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Pixel/SizedFormat.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Pixel/SizedFormatHelper.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Pixel/UnsizedFormat.hpp)

set(MSG_PIXEL_DESCRIPTOR_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PixelDescriptor.cpp)

add_library(MSG-PixelDescriptor ${MSG_PIXEL_DESCRIPTOR_SOURCE_FILES} ${MSG_PIXEL_DESCRIPTOR_HEADER_FILES})
add_library(MSG::PixelDescriptor ALIAS MSG-PixelDescriptor)
target_include_directories(MSG-PixelDescriptor PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-PixelDescriptor MSG::Core)

set_target_properties(
  MSG-PixelDescriptor
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
