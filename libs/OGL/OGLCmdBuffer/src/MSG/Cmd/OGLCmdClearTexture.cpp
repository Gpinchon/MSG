#include <MSG/Cmd/OGLCmdClearTexture.hpp>
#include <MSG/OGLTexture.hpp>

#include <GL/glew.h>

GLenum GetFormat(const Msg::OGLClearValue& a_Value)
{
    switch (a_Value.index()) {
    case 0:
        return GL_RGBA;
    case 1:
    case 2:
        return GL_RGBA_INTEGER;
    }
    return GL_NONE;
}

GLenum GetType(const Msg::OGLClearValue& a_Value)
{
    switch (a_Value.index()) {
    case 0:
        return GL_FLOAT;
    case 1:
        return GL_INT;
    case 2:
        return GL_UNSIGNED_INT;
    }
    return GL_NONE;
}

Msg::OGLCmdClearTexture::OGLCmdClearTexture(const std::shared_ptr<OGLTexture>& a_Txt, const OGLClearTextureInfo& a_Info)
    : _txt(a_Txt)
    , _level(a_Info.level)
    , _offset(a_Info.offset)
    , _size(a_Info.size)
    , _type(GetType(a_Info.value))
    , _format(GetFormat(a_Info.value))
    , _value(a_Info.value)
{
}

void Msg::OGLCmdClearTexture::operator()(OGLCmdBufferExecutionState&) const
{
    glClearTexSubImage(
        _txt->handle, _level,
        _offset[0], _offset[1], _offset[2],
        _size[0], _size[1], _size[2],
        _format, _type, &_value);
}