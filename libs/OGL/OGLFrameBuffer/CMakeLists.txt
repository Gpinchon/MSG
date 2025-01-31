Fetch_GLEW()

set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLFrameBuffer.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLFrameBuffer.cpp)

add_library(MSG-OGLFrameBuffer STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLFrameBuffer ALIAS MSG-OGLFrameBuffer)
target_include_directories(MSG-OGLFrameBuffer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLFrameBuffer
  MSG::OGLContext
  MSG::OGLTexture
  ${OPENGL_LIBRARIES})
set_target_properties(
  MSG-OGLFrameBuffer
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()