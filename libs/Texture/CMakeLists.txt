Fetch_FasTC()

set(MSG_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Texture/Sampler.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Texture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Sampler.hpp)

set(MSG_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Texture.cpp)

add_library(MSG-Texture ${MSG_TEXTURE_SOURCE_FILES} ${MSG_TEXTURE_HEADER_FILES})
add_library(MSG::Texture ALIAS MSG-Texture)
target_include_directories(MSG-Texture PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  ${FASTCBASE_INCLUDES}
  ${FASTCCORE_INCLUDES})
target_link_libraries(MSG-Texture
  MSG::Image
  FasTCBase
  FasTCCore
  FasTCIO)

set_target_properties(
  MSG-Texture
  PROPERTIES FOLDER "MSG")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
endif()
