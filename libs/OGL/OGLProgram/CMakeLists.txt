Fetch_GLEW()

set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLProgram.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLProgram.cpp)

add_library(MSG-OGLProgram STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLProgram ALIAS MSG-OGLProgram)
target_include_directories(MSG-OGLProgram PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLProgram
  MSG::OGLContext::Interface
  MSG::OGLShader
  GLEW::GLEW)
set_target_properties(
  MSG-OGLProgram
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()