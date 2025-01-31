Fetch_GLEW()

set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLShader.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLShader.cpp)

add_library(MSG-OGLShader STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLShader ALIAS MSG-OGLShader)
target_include_directories(MSG-OGLShader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLShader
  MSG::OGLContext
  ${OPENGL_LIBRARIES})
set_target_properties(
  MSG-OGLShader
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()