Fetch_GLEW()

set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLDebugGroup.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLDebugGroup.cpp)

add_library(MSG-OGLDebugGroup STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLDebugGroup ALIAS MSG-OGLDebugGroup)
target_include_directories(MSG-OGLDebugGroup PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLDebugGroup
  MSG::OGLContext
  ${OPENGL_LIBRARIES})
set_target_properties(
  MSG-OGLDebugGroup
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()