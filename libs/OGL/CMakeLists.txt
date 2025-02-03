add_subdirectory(OGLContext)
add_subdirectory(OGLDebugGroup)
add_subdirectory(OGLBuffer)
add_subdirectory(OGLVertexArray)
add_subdirectory(OGLTexture)
add_subdirectory(OGLSampler)
add_subdirectory(OGLFrameBuffer)
add_subdirectory(OGLShader)
add_subdirectory(OGLProgram)
add_subdirectory(OGLRenderPass)

add_library(MSG-OGL INTERFACE)
add_library(MSG::OGL ALIAS MSG-OGL)
target_link_libraries(MSG-OGL INTERFACE
    MSG::OGLContext
    MSG::OGLDebugGroup
    MSG::OGLBuffer
    MSG::OGLVertexArray
    MSG::OGLTexture
    MSG::OGLSampler
    MSG::OGLFrameBuffer
    MSG::OGLShader
    MSG::OGLProgram
    MSG::OGLRenderPass)