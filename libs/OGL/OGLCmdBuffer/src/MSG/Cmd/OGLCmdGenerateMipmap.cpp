#include <MSG/Cmd/OGLCmdGenerateMipmap.hpp>

#include <MSG/OGLTexture.hpp>

#include <GL/glew.h>

void MSG::OGLCmdGenerateMipmap::operator()(OGLCmdBufferExecutionState& a_State) const
{
    glGenerateTextureMipmap(*_txt);
}