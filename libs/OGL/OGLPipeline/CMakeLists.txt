
set(MSG_OGL_RENDERPASS_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLPipelineInfo.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLPipeline.hpp)
set(MSG_OGL_RENDERPASS_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLPipeline.cpp)

add_library(MSG-OGLPipeline STATIC ${MSG_OGL_RENDERPASS_HEADER_FILES} ${MSG_OGL_RENDERPASS_SOURCE_FILES})
add_library(MSG::OGLPipeline ALIAS MSG-OGLPipeline)
target_include_directories(MSG-OGLPipeline PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLPipeline
  MSG::OGLBuffer
  MSG::OGLDebugGroup
  MSG::OGLProgram
  MSG::OGLSampler
  MSG::OGLTexture
  MSG::OGLBindlessTextureSampler
  MSG::OGLVertexArray)
set_target_properties(
  MSG-OGLPipeline
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()