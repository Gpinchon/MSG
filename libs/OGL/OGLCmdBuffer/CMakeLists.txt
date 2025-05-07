set(MSG_OGL_TEXTURE_HEADER_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cmd/OGLCmdClearTexture.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cmd/OGLCmdDispatchCompute.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cmd/OGLCmdDraw.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cmd/OGLCmdEndRenderPass.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cmd/OGLCmdPushCmdBuffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cmd/OGLCmdPushPipeline.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/Cmd/OGLCmdPushRenderPass.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLCmd.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLCmdBuffer.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/MSG/OGLCmdBufferExecutionState.hpp)
set(MSG_OGL_TEXTURE_SOURCE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cmd/OGLCmdClearTexture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cmd/OGLCmdDispatchCompute.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cmd/OGLCmdDraw.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cmd/OGLCmdEndRenderPass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cmd/OGLCmdPushCmdBuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cmd/OGLCmdPushPipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/Cmd/OGLCmdPushRenderPass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/MSG/OGLCmdBuffer.cpp)

add_library(MSG-OGLCmdBuffer STATIC ${MSG_OGL_TEXTURE_HEADER_FILES} ${MSG_OGL_TEXTURE_SOURCE_FILES})
add_library(MSG::OGLCmdBuffer ALIAS MSG-OGLCmdBuffer)
target_include_directories(MSG-OGLCmdBuffer PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(MSG-OGLCmdBuffer
  MSG::OGLContext
  MSG::OGLFence
  MSG::OGLTexture
  MSG::OGLPipeline
  MSG::OGLRenderPass)
set_target_properties(
  MSG-OGLCmdBuffer
  PROPERTIES FOLDER "MSG/OGL")

if(MSVC_IDE)
  GroupSources(src)
  GroupSources(include)
  GroupSources(inline)
endif()