
set(MSG_OGL_RENDERPASS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLRenderPass/Info.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLRenderPass.hpp)
set(MSG_OGL_RENDERPASS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLRenderPass.cpp)

add_library(MSG-OGLRenderPass STATIC ${MSG_OGL_RENDERPASS_HEADER_FILES} ${MSG_OGL_RENDERPASS_SOURCE_FILES})
add_library(MSG::OGLRenderPass ALIAS MSG-OGLRenderPass)
target_include_directories(MSG-OGLRenderPass PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLRenderPass
  MSG::OGLBuffer
  MSG::OGLDebugGroup
  MSG::OGLFrameBuffer
  MSG::OGLProgram
  MSG::OGLSampler
  MSG::OGLTexture
  MSG::OGLVertexArray)
set_target_properties(
  MSG-OGLRenderPass
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()