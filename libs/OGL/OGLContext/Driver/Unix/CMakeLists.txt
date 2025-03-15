cmake_policy(SET CMP0072 NEW)

set(MSG_OGL_CONTEXT_LIBS ${OPENGL_LIBRARIES})

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLContext)
set(SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLContext)

set(MSG_OGL_CONTEXT_IMPL_HEADER_FILES ${MSG_OGL_CONTEXT_IMPL_HEADER_FILES}
  ${INCLUDE_DIR}/PlatformCtx.hpp
  ${INCLUDE_DIR}/GLX.hpp
  ${INCLUDE_DIR}/X11.hpp)
set(MSG_OGL_CONTEXT_IMPL_SOURCE_FILES ${MSG_OGL_CONTEXT_IMPL_SOURCE_FILES}
  ${SOURCE_DIR}/PlatformCtx.cpp
  ${SOURCE_DIR}/GLX.cpp
  ${SOURCE_DIR}/X11.cpp)

find_package(X11 REQUIRED)
message("X11_FOUND         : ${X11_FOUND}")
message("X11_INCLUDE_DIR   : ${X11_INCLUDE_DIR}")
message("X11_LIBRARIES     : ${X11_LIBRARIES}")

#check if we need to add xcb dependencies
check_library_exists("${X11_LIBRARIES}" "xcb_get_setup" "${X11_LIBRARY_DIR}" X11_SOLO)
if (NOT X11_SOLO)
  set(X11_LIBRARIES ${X11_LIBRARIES} xcb)
endif (NOT X11_SOLO)

add_library(MSG-OGLContext-Implementation STATIC
  ${MSG_OGL_CONTEXT_IMPL_SOURCE_FILES}
  ${MSG_OGL_CONTEXT_IMPL_HEADER_FILES})
add_library(MSG::OGLContext::Implementation ALIAS MSG-OGLContext-Implementation)
target_include_directories(MSG-OGLContext-Implementation PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLContext-Implementation PUBLIC
  MSG::OGLContext::Interface
  MSG::PixelDescriptor
  MSG::Debug
  ${X11_LIBRARIES})

set_target_properties(
  MSG-OGLContext-Implementation
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  set(CMAKE_DEBUG_POSTFIX "d")
  # Run macro
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()
