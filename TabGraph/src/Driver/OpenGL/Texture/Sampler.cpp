/*
* @Author: gpinchon
* @Date:   2021-05-01 20:04:17
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-03 17:23:39
*/

#include "Driver/OpenGL/Texture/Sampler.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>

using namespace TabGraph;

namespace OpenGL {
unsigned GetEnum(Textures::Sampler::Wrap value)
{
    switch (value) {
    case Textures::Sampler::Wrap::Repeat:
        return GL_REPEAT;
    case Textures::Sampler::Wrap::ClampToBorder:
        return GL_CLAMP_TO_BORDER;
    case Textures::Sampler::Wrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case Textures::Sampler::Wrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    case Textures::Sampler::Wrap::MirroredClampToEdge:
        return GL_MIRROR_CLAMP_TO_EDGE;
    default:
        throw std::runtime_error("Unknown Textures::Sampler::Wrap");
    }
}

unsigned GetEnum(TabGraph::Textures::Sampler::Filter value)
{
    switch (value) {
    case Textures::Sampler::Filter::Nearest:
        return GL_NEAREST;
    case Textures::Sampler::Filter::Linear:
        return GL_LINEAR;
    case Textures::Sampler::Filter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case Textures::Sampler::Filter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case Textures::Sampler::Filter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case Textures::Sampler::Filter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    default:
        throw std::runtime_error("Unknown Textures::Sampler::Filter");
    }
}

unsigned GetEnum(::Textures::Sampler::CompareMode value)
{
    switch (value) {
    case Textures::Sampler::CompareMode::None:
        return GL_NONE;
    case Textures::Sampler::CompareMode::CompareRefToTexture:
        return GL_COMPARE_REF_TO_TEXTURE;
    default:
        throw std::runtime_error("Unknown Textures::Sampler::CompareMode");
    }
}

unsigned GetEnum(::Textures::Sampler::CompareFunc value)
{
    switch (value) {
    case Textures::Sampler::CompareFunc::LessEqual:
        return GL_LEQUAL;
    case Textures::Sampler::CompareFunc::GreaterEqual:
        return GL_GEQUAL;
    case Textures::Sampler::CompareFunc::Less:
        return GL_LESS;
    case Textures::Sampler::CompareFunc::Greater:
        return GL_GREATER;
    case Textures::Sampler::CompareFunc::Equal:
        return GL_EQUAL;
    case Textures::Sampler::CompareFunc::NotEqual:
        return GL_NOTEQUAL;
    case Textures::Sampler::CompareFunc::Always:
        return GL_ALWAYS;
    case Textures::Sampler::CompareFunc::Never:
        return GL_NEVER;
    default:
        throw std::runtime_error("Unknown Textures::Sampler::CompareFunc");
    }
}
};

static inline auto GetWrapEnum(GLenum value)
{
    switch (value) {
    case GL_REPEAT:
        return Textures::Sampler::Wrap::Repeat;
    case GL_CLAMP_TO_BORDER:
        return Textures::Sampler::Wrap::ClampToBorder;
    case GL_CLAMP_TO_EDGE:
        return Textures::Sampler::Wrap::ClampToEdge;
    case GL_MIRRORED_REPEAT:
        return Textures::Sampler::Wrap::MirroredRepeat;
    case GL_MIRROR_CLAMP_TO_EDGE:
        return Textures::Sampler::Wrap::MirroredClampToEdge;
    default:
        throw std::runtime_error("Unknown Wrap Enum");
    }
}

static inline auto GetFilterEnum(GLenum value)
{
    switch (value) {
    case GL_NEAREST:
        return Textures::Sampler::Filter::Nearest;
    case GL_LINEAR:
        return Textures::Sampler::Filter::Linear;
    case GL_NEAREST_MIPMAP_LINEAR:
        return Textures::Sampler::Filter::NearestMipmapLinear;
    case GL_NEAREST_MIPMAP_NEAREST:
        return Textures::Sampler::Filter::NearestMipmapNearest;
    case GL_LINEAR_MIPMAP_LINEAR:
        return Textures::Sampler::Filter::LinearMipmapLinear;
    case GL_LINEAR_MIPMAP_NEAREST:
        return Textures::Sampler::Filter::LinearMipmapNearest;
    }
    return Textures::Sampler::Filter::Unknown;
}

static inline auto GetCompareModeEnum(GLenum value)
{
    switch (value) {
    case GL_NONE:
        return Textures::Sampler::CompareMode::None;
    case GL_COMPARE_REF_TO_TEXTURE:
        return Textures::Sampler::CompareMode::CompareRefToTexture;
    }
    return Textures::Sampler::CompareMode::Unknown;
}

static inline auto GetCompareFuncEnum(GLenum value)
{
    switch (value) {
    case GL_LEQUAL:
        return Textures::Sampler::CompareFunc::LessEqual;
    case GL_GEQUAL:
        return Textures::Sampler::CompareFunc::GreaterEqual;
    case GL_LESS:
        return Textures::Sampler::CompareFunc::Less;
    case GL_GREATER:
        return Textures::Sampler::CompareFunc::Greater;
    case GL_EQUAL:
        return Textures::Sampler::CompareFunc::Equal;
    case GL_NOTEQUAL:
        return Textures::Sampler::CompareFunc::NotEqual;
    case GL_ALWAYS:
        return Textures::Sampler::CompareFunc::Always;
    case GL_NEVER:
        return Textures::Sampler::CompareFunc::Never;
    }
    return Textures::Sampler::CompareFunc::Unknown;
}

Textures::Sampler::Impl::Impl()
{
    glGenSamplers(1, &_handle);
}

Textures::Sampler::Impl::~Impl()
{
    glDeleteSamplers(1, &_handle);
}

Textures::Sampler::Impl::Handle Textures::Sampler::Impl::GetHandle() const
{
    return _handle;
}

Textures::Sampler::Filter Textures::Sampler::Impl::GetMagFilter() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_MAG_FILTER, &value);
    return GetFilterEnum(value);
}

Textures::Sampler::Filter Textures::Sampler::Impl::GetMinFilter() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_MIN_FILTER, &value);
    return GetFilterEnum(value);
}

float Textures::Sampler::Impl::GetMinLOD() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_MIN_LOD, &value);
    return value;
}

float Textures::Sampler::Impl::GetMaxLOD() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_MAX_LOD, &value);
    return value;
}

float Textures::Sampler::Impl::GetLODBias() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_LOD_BIAS, &value);
    return value;
}

Textures::Sampler::Wrap Textures::Sampler::Impl::GetWrapS() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_WRAP_S, &value);
    return GetWrapEnum(value);
}

Textures::Sampler::Wrap Textures::Sampler::Impl::GetWrapT() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_WRAP_T, &value);
    return GetWrapEnum(value);
}

Textures::Sampler::Wrap Textures::Sampler::Impl::GetWrapR() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_WRAP_R, &value);
    return GetWrapEnum(value);
}

Textures::Sampler::CompareMode Textures::Sampler::Impl::GetCompareMode() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_COMPARE_MODE, &value);
    return GetCompareModeEnum(value);
}

Textures::Sampler::CompareFunc Textures::Sampler::Impl::GetCompareFunc() const
{
    int32_t value;
    glGetSamplerParameteriv(_handle, GL_TEXTURE_COMPARE_FUNC, &value);
    return GetCompareFuncEnum(value);
}

float Textures::Sampler::Impl::GetMaxAnisotropy() const
{
    float value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_MAX_ANISOTROPY, &value);
    return value;
}

glm::vec4 Textures::Sampler::Impl::GetBorderColor() const
{
    glm::vec4 value;
    glGetSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
    return value;
}

void Textures::Sampler::Impl::SetMagFilter(Filter value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, OpenGL::GetEnum(value));
}

void Textures::Sampler::Impl::SetMinFilter(Filter value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, OpenGL::GetEnum(value));
}

void Textures::Sampler::Impl::SetMinLOD(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_MIN_LOD, value);
}

void Textures::Sampler::Impl::SetMaxLOD(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_MAX_LOD, value);
}

void Textures::Sampler::Impl::SetLODBias(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_LOD_BIAS, value);
}

void Textures::Sampler::Impl::SetWrapS(Wrap value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, OpenGL::GetEnum(value));
}

void Textures::Sampler::Impl::SetWrapT(Wrap value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, OpenGL::GetEnum(value));
}

void Textures::Sampler::Impl::SetWrapR(Wrap value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, OpenGL::GetEnum(value));
}

void Textures::Sampler::Impl::SetCompareMode(CompareMode value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_MODE, OpenGL::GetEnum(value));
}

void Textures::Sampler::Impl::SetCompareFunc(CompareFunc value)
{
    glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_FUNC, OpenGL::GetEnum(value));
}

void Textures::Sampler::Impl::SetMaxAnisotropy(float value)
{
    glSamplerParameterf(_handle, GL_TEXTURE_MAX_ANISOTROPY, value);
}

void Textures::Sampler::Impl::SetBorderColor(glm::vec4 value)
{
    glSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(value));
}
