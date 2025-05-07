add_subdirectory(OGLContext)
add_subdirectory(OGLDebugGroup)
add_subdirectory(OGLFence)
add_subdirectory(OGLBuffer)
add_subdirectory(OGLTypedBuffer)
add_subdirectory(OGLVertexArray)
add_subdirectory(OGLTexture)
add_subdirectory(OGLSampler)
add_subdirectory(OGLFrameBuffer)
add_subdirectory(OGLShader)
add_subdirectory(OGLProgram)
add_subdirectory(OGLPipeline)
add_subdirectory(OGLRenderPass)
add_subdirectory(OGLCmdBuffer)

add_library(MSG-OGL INTERFACE)
add_library(MSG::OGL ALIAS MSG-OGL)
target_link_libraries(MSG-OGL INTERFACE
    MSG::OGLContext
    MSG::OGLFence
    MSG::OGLCmdBuffer
    MSG::OGLDebugGroup
    MSG::OGLBuffer
    MSG::OGLTypedBuffer
    MSG::OGLVertexArray
    MSG::OGLTexture
    MSG::OGLSampler
    MSG::OGLFrameBuffer
    MSG::OGLShader
    MSG::OGLProgram
    MSG::OGLPipeline
    MSG::OGLRenderPass)