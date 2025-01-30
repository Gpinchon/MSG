Fetch_GLEW()

set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTexture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTexture2D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTextureCubemap.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTexture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTexture2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTextureCubemap.cpp)

add_library(MSG-OGLTexture STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLTexture ALIAS MSG-OGLTexture)
target_include_directories(MSG-OGLTexture PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLTexture
  MSG::OGLContext::Interface
  MSG::Texture
  GLEW::GLEW)
set_target_properties(
  MSG-OGLTexture
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()