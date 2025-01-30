Fetch_GLEW()

set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLBuffer.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLBuffer.cpp)

add_library(MSG-OGLBuffer STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLBuffer ALIAS MSG-OGLBuffer)
target_include_directories(MSG-OGLBuffer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLBuffer
  MSG::OGLContext::Interface
  GLEW::GLEW)
set_target_properties(
  MSG-OGLBuffer
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()