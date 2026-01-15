#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture.hpp>

#include <MSG/ToGL.hpp>

#include <MSG/Image.hpp>

#include <GL/glew.h>

namespace Msg {
static inline auto CreateTexture(OGLContext& a_Context, const GLenum& a_Target)
{
    GLuint handle = 0;
    ExecuteOGLCommand(a_Context, [&handle, &a_Target] { glCreateTextures(a_Target, 1, &handle); }, true);
    return handle;
}

OGLTexture::OGLTexture(OGLContext& a_Context, const OGLTextureInfo& a_Info, const bool& a_Allocate)
    : OGLTextureInfo(a_Info)
    , handle(a_Allocate ? CreateTexture(a_Context, a_Info.target) : 0)
    , context(a_Context)
{
    if (a_Info.sparse)
        ExecuteOGLCommand(a_Context, [handle = handle] { 
            assert(GLEW_ARB_sparse_texture && GLEW_ARB_sparse_texture2);
            glTextureParameteri(handle, GL_TEXTURE_SPARSE_ARB, GL_TRUE); });
    if (a_Allocate)
        Allocate();
}

OGLTexture::~OGLTexture()
{
    ExecuteOGLCommand(context, [handle = handle] { glDeleteTextures(1, &handle); });
}

void Msg::OGLTexture::CommitPage(const OGLTextureCommitInfo& a_Info)
{
    ExecuteOGLCommand(context, [handle = handle, info = a_Info] {
        glTexturePageCommitmentEXT(
            handle,
            info.level,
            info.offsetX, info.offsetY, info.offsetZ,
            info.width, info.height, info.depth,
            info.commit);
    });
}

void OGLTexture::Allocate()
{
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_PROXY_TEXTURE_1D:
        ExecuteOGLCommand(context,
            [handle = handle, info = OGLTextureInfo(*this)] {
                glTextureStorage1D(handle, info.levels, info.sizedFormat,
                    info.width);
            });
        break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_1D_ARRAY:
    case GL_TEXTURE_RECTANGLE:
    case GL_TEXTURE_CUBE_MAP:
    case GL_PROXY_TEXTURE_2D:
    case GL_PROXY_TEXTURE_1D_ARRAY:
    case GL_PROXY_TEXTURE_RECTANGLE:
    case GL_PROXY_TEXTURE_CUBE_MAP:
        ExecuteOGLCommand(context,
            [handle = handle, info = OGLTextureInfo(*this)] {
                glTextureStorage2D(handle, info.levels, info.sizedFormat,
                    info.width, info.height);
            });
        break;
    case GL_TEXTURE_3D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
    case GL_PROXY_TEXTURE_3D:
    case GL_PROXY_TEXTURE_2D_ARRAY:
    case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
        ExecuteOGLCommand(context,
            [handle = handle, info = OGLTextureInfo(*this)] {
                glTextureStorage3D(handle, info.levels, info.sizedFormat,
                    info.width, info.height, info.depth);
            });
        break;
    default:
        break;
    }
}

void OGLTexture::Clear(
    const uint32_t& a_Format,
    const uint32_t& a_Type,
    const void* a_Data) const
{
    auto clearFunc = [handle = handle, format = a_Format, type = a_Type, data = a_Data] {
        glClearTexImage(
            handle,
            0, // level
            format, type, data);
    };
    ExecuteOGLCommand(context, clearFunc, true);
}

void Msg::OGLTexture::DownloadLevel(
    const uint32_t& a_Level,
    const uint32_t& a_Format,
    const uint32_t& a_Type,
    const uint32_t& a_BufferSize,
    void* a_Data) const
{
    ExecuteOGLCommand(
        context, [&] {
            glGetTextureImage(handle, a_Level, a_Format, a_Type, a_BufferSize, a_Data);
        },
        true);
}

void OGLTexture::UploadLevel(
    const uint32_t& a_Level,
    const Image& a_Src) const
{
    OGLTextureUploadInfo info {
        .level           = a_Level,
        .width           = a_Src.GetSize().x,
        .height          = a_Src.GetSize().y,
        .depth           = a_Src.GetSize().z,
        .pixelDescriptor = a_Src.GetPixelDescriptor()
    };
    return UploadLevel(info, a_Src.Read());
}

void OGLTexture::UploadLevel(
    const uint32_t& a_Level,
    const glm::uvec3& a_SrcOffset,
    const glm::uvec3& a_SrcSize,
    const Image& a_Src) const
{
    OGLTextureUploadInfo info {
        .level           = a_Level,
        .offsetX         = a_SrcOffset.x,
        .offsetY         = a_SrcOffset.y,
        .offsetZ         = a_SrcOffset.z,
        .width           = a_SrcSize.x,
        .height          = a_SrcSize.y,
        .depth           = a_SrcSize.z,
        .pixelDescriptor = a_Src.GetPixelDescriptor()
    };
    return UploadLevel(info, a_Src.Read(a_SrcOffset, a_SrcSize));
}

void OGLTexture::UploadLevel(const OGLTextureUploadInfo& a_Info, std::vector<std::byte> a_Data) const
{
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_PROXY_TEXTURE_1D:
        ExecuteOGLCommand(context,
            [handle = handle, info = a_Info, data = std::move(a_Data)] {
                if (info.pixelDescriptor.GetSizedFormat() == Msg::PixelSizedFormat::DXT5_RGBA) {
                    glCompressedTextureSubImage1D(
                        handle,
                        info.level,
                        info.offsetX,
                        info.width,
                        ToGL(info.pixelDescriptor.GetSizedFormat()),
                        GLsizei(data.size()),
                        data.data());
                } else {
                    const auto dataFormat = ToGL(info.pixelDescriptor.GetUnsizedFormat());
                    const auto dataType   = ToGL(info.pixelDescriptor.GetDataType());
                    glTextureSubImage1D(
                        handle,
                        info.level,
                        info.offsetX,
                        info.width,
                        dataFormat, dataType,
                        data.data());
                }
            });
        break;
    case GL_TEXTURE_2D:
    case GL_TEXTURE_1D_ARRAY:
    case GL_TEXTURE_RECTANGLE:
    case GL_PROXY_TEXTURE_2D:
    case GL_PROXY_TEXTURE_1D_ARRAY:
    case GL_PROXY_TEXTURE_RECTANGLE:
        ExecuteOGLCommand(context,
            [handle = handle, info = a_Info, data = std::move(a_Data)] {
                if (info.pixelDescriptor.GetSizedFormat() == Msg::PixelSizedFormat::DXT5_RGBA) {
                    glCompressedTextureSubImage2D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY,
                        info.width, info.height,
                        ToGL(info.pixelDescriptor.GetSizedFormat()),
                        GLsizei(data.size()),
                        data.data());
                } else {
                    const auto dataFormat = ToGL(info.pixelDescriptor.GetUnsizedFormat());
                    const auto dataType   = ToGL(info.pixelDescriptor.GetDataType());
                    glTextureSubImage2D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY,
                        info.width, info.height,
                        dataFormat, dataType,
                        data.data());
                }
            });
        break;
    case GL_TEXTURE_3D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_CUBE_MAP:
    case GL_TEXTURE_CUBE_MAP_ARRAY:
    case GL_PROXY_TEXTURE_3D:
    case GL_PROXY_TEXTURE_2D_ARRAY:
    case GL_PROXY_TEXTURE_CUBE_MAP:
    case GL_PROXY_TEXTURE_CUBE_MAP_ARRAY:
        ExecuteOGLCommand(context,
            [handle = handle, info = a_Info, data = std::move(a_Data)] {
                if (info.pixelDescriptor.GetSizedFormat() == Msg::PixelSizedFormat::DXT5_RGBA) {
                    glCompressedTextureSubImage3D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY, info.offsetZ,
                        info.width, info.height, info.depth,
                        ToGL(info.pixelDescriptor.GetSizedFormat()),
                        GLsizei(data.size()),
                        data.data());
                } else {
                    const auto dataFormat = ToGL(info.pixelDescriptor.GetUnsizedFormat());
                    const auto dataType   = ToGL(info.pixelDescriptor.GetDataType());
                    glTextureSubImage3D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY, info.offsetZ,
                        info.width, info.height, info.depth,
                        dataFormat, dataType,
                        data.data());
                }
            });
        break;
    default:
        break;
    }
}
}
