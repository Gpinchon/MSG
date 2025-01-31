cmake_policy(SET CMP0072 NEW)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLContext/Win32)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLContext/Win32)

set(MSG_OGL_CONTEXT_IMPL_HEADER_FILES ${MSG_OGL_CONTEXT_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/PlatformCtx.hpp
  ${INCLUDE_DIR}/WGL.hpp
  ${INCLUDE_DIR}/Win32.hpp)
set(MSG_OGL_CONTEXT_IMPL_SOURCE_FILES ${MSG_OGL_CONTEXT_IMPL_SOURCE_FILES}
  ${SOURCE_DIR}/PlatformCtx.cpp
  ${SOURCE_DIR}/WGL.cpp
  ${SOURCE_DIR}/Win32.cpp)

add_library(MSG-OGLContext-Implementation STATIC
  ${MSG_OGL_CONTEXT_IMPL_SOURCE_FILES}
  ${MSG_OGL_CONTEXT_IMPL_HEADER_FILES})
add_library(MSG::OGLContext::Implementation ALIAS MSG-OGLContext-Implementation)
target_compile_definitions(MSG-OGLContext-Implementation PUBLIC GLEW_NO_GLU)
target_compile_definitions(MSG-OGLContext-Implementation PUBLIC GLEW_STATIC)
target_include_directories(MSG-OGLContext-Implementation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLContext-Implementation
  MSG::OGLContext
  ${OPENGL_LIBRARIES})

set_target_properties(
  MSG-OGLContext-Implementation
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
