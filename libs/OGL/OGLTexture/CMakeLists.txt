set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTexture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTexture2D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTexture2DArray.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTexture3D.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTextureCube.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTextureCubeArray.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLTextureView.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTexture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTexture2D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTexture2DArray.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTexture3D.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTextureCube.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTextureCubeArray.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLTextureView.cpp)

add_library(MSG-OGLTexture STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLTexture ALIAS MSG-OGLTexture)
target_include_directories(MSG-OGLTexture PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLTexture
  MSG::OGLContext
  MSG::Texture)
set_target_properties(
  MSG-OGLTexture
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()