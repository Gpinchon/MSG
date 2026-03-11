#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture.hpp>

#include <MSG/Debug.hpp>
#include <MSG/Image.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>

namespace Msg {
Msg::OGLTexture::OGLTexture(OGLContext& a_Context)
    : context(a_Context)
{
}

OGLTexture::OGLTexture(OGLContext& a_Context, const OGLTextureInfo& a_Info, const bool& a_Allocate)
    : OGLTexture(a_Context)
{
    if (a_Allocate)
        Initialize(a_Info);
    else
        ((OGLTextureInfo&)*this) = a_Info;
}

Msg::OGLTexture::OGLTexture(OGLTexture&& a_Other)
    : handle(a_Other.handle)
    , context(a_Other.context)
{
    handle = 0;
}

OGLTexture::~OGLTexture()
{
    if (handle != 0)
        ExecuteOGLCommand(context, [handle = handle] { glDeleteTextures(1, &handle); });
}

void Msg::OGLTexture::Initialize(const OGLTextureInfo& a_Info)
{
    ((OGLTextureInfo&)*this) = a_Info;
    handle                   = OGLTexture::Create(context, a_Info.target);
    if (a_Info.sparse)
        ExecuteOGLCommand(context, [handle = handle] {
            static bool supportChecked = false;
            if (!supportChecked) {
                MSGCheckErrorWarning(
                    !GLEW_ARB_sparse_texture || !GLEW_ARB_sparse_texture2,
                    "Sparse textures extension support not advertised by current context, proceed at your own risk !");
                supportChecked = true;
            }
            glTextureParameteri(handle, GL_TEXTURE_SPARSE_ARB, GL_TRUE); });
    Allocate();
}

void Msg::OGLTexture::GenerateMipmap() const
{
    ExecuteOGLCommand(context, [handle = handle] {
        glGenerateTextureMipmap(handle);
    });
}

uint32_t Msg::OGLTexture::SparseLevels() const
{
    uint32_t sparseLevels = 0;
    if (sparse) {
        ExecuteOGLCommand(context, [handle = handle, &sparseLevels] { //
            glGetTextureParameterIuiv(handle, GL_NUM_SPARSE_LEVELS_ARB, &sparseLevels);
        },
            true);
    }
    return sparseLevels;
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
    const uint32_t& a_Level,
    const void* a_Data) const
{
    auto clearFunc = [handle = handle, lvl = a_Level, format = a_Format, type = a_Type, data = a_Data] {
        glClearTexImage(
            handle,
            lvl,
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

OGLTextureFormatSparseInfo OGLTexture::GetFormatSparseInfo(OGLContext& a_Context,
    const uint32_t& a_TextureTarget, const uint32_t& a_SizedFormat)
{
    Msg::OGLTextureFormatSparseInfo info;
    info.sizedFormat = a_SizedFormat;
    ExecuteOGLCommand(a_Context, [&a_TextureTarget, &a_SizedFormat, &info] {
        int32_t pageSizes = 0;
        glGetInternalformativ(a_TextureTarget, a_SizedFormat, GL_NUM_VIRTUAL_PAGE_SIZES_ARB, 1, &pageSizes);
        glGetInternalformativ(a_TextureTarget, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_X_ARB, 1, &info.pageWidth);
        glGetInternalformativ(a_TextureTarget, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_Y_ARB, 1, &info.pageHeight);
        glGetInternalformativ(a_TextureTarget, a_SizedFormat, GL_VIRTUAL_PAGE_SIZE_Z_ARB, 1, &info.pageDepth);
        info.supported = pageSizes > 0; //
    },
        true);
    return info;
}

uint32_t OGLTexture::Create(OGLContext& a_Context, const uint32_t& a_Target)
{
    GLuint handle = 0;
    ExecuteOGLCommand(a_Context, [&handle, &a_Target] { glCreateTextures(a_Target, 1, &handle); }, true);
    return handle;
}
}
