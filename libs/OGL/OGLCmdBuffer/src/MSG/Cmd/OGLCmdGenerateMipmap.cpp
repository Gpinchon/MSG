#include <MSG/Cmd/OGLCmdGenerateMipmap.hpp>

#include <MSG/OGLTexture.hpp>

#include <GL/glew.h>

void Msg::OGLCmdGenerateMipmap::operator()(OGLCmdBufferExecutionState& a_State) const
{
    glGenerateTextureMipmap(*_txt);
}