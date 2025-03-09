set(MSG_IMAGE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/BRDF.hpp)

set(MSG_IMAGE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/BRDF.cpp)

add_library(MSG-BRDF ${MSG_IMAGE_SOURCE_FILES} ${MSG_IMAGE_HEADER_FILES})
add_library(MSG::BRDF ALIAS MSG-BRDF)
target_include_directories(MSG-BRDF PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(MSG-BRDF
  MSG::Tools
  MSG::Texture)

set_target_properties(
  MSG-BRDF
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
