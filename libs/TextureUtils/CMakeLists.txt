Fetch_FasTC()

set(HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/TextureUtils.hpp)

set(SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/TextureUtils.cpp)

add_library(MSG-TextureUtils ${SOURCE_FILES} ${HEADER_FILES})
add_library(MSG::TextureUtils ALIAS MSG-TextureUtils)
target_include_directories(MSG-TextureUtils PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(MSG-TextureUtils
  MSG::Texture
  MSG::ImageUtils
  FasTCBase
  FasTCCore
  FasTCIO)

set_target_properties(
  MSG-TextureUtils
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
endif()
