#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture.hpp>

#include <MSG/Debug.hpp>
#include <MSG/Image.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>

uint32_t GetFormatComponents(const GLenum& a_Format)
{
    switch (a_Format) {
    case GL_RED:
    case GL_RED_INTEGER:
        return 1;
    case GL_RG:
    case GL_RG_INTEGER:
        return 2;
    case GL_RGB:
    case GL_RGB_INTEGER:
        return 3;
    case GL_RGBA:
    case GL_RGBA_INTEGER:
        return 4;
    default:
        MSGErrorFatal("Unknown OGL pixel format");
        return -1u;
    }
}

uint32_t GetTypeSize(const GLenum& a_Type)
{
    switch (a_Type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        return 1;
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        return 2;
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        return 4;
    default:
        MSGErrorFatal("Unknown OGL data type");
        return -1u;
    }
}

bool GetTypeCompressed(const GLenum& a_Type)
{
    return a_Type == GL_RGBA_DXT5_S3TC;
}

uint32_t GetPixelSize(const GLenum& a_Format, const GLenum& a_Type)
{
    return GetFormatComponents(a_Format) * GetTypeSize(a_Type);
}

uint32_t GetPixelBufferSize(const GLenum& a_Format, const GLenum& a_Type, const glm::uvec3& a_Pixels)
{
    if (GetTypeCompressed(a_Type)) {
        constexpr size_t blockSize    = 16;
        constexpr glm::uvec3 blockDim = { 4, 4, 1 };
        const glm::uvec3 blockCount   = (a_Pixels + (blockDim - 1u)) / blockDim;
        const size_t blocksNbr        = blockCount.x * blockCount.y * blockCount.z;
        return blockSize * blocksNbr;
    } else
        return GetPixelSize(a_Format, a_Type) * a_Pixels.x * a_Pixels.y * a_Pixels.z;
}

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
    auto& pxDsc = a_Src.GetPixelDescriptor();
    OGLTextureUploadInfo info;
    info.level  = a_Level;
    info.width  = a_Src.GetSize().x;
    info.height = a_Src.GetSize().y;
    info.depth  = a_Src.GetSize().z;
    if (pxDsc.IsCompressed()) {
        info.format = GL_RGBA_DXT5_S3TC;
    } else {
        info.format = ToGL(a_Src.GetPixelDescriptor().GetUnsizedFormat());
        info.type   = ToGL(a_Src.GetPixelDescriptor().GetDataType());
    }
    return UploadLevel(info, a_Src.Read());
}

void OGLTexture::UploadLevel(
    const uint32_t& a_Level,
    const uint32_t& a_SrcOffsetX, const uint32_t& a_SrcOffsetY, const uint32_t& a_SrcOffsetZ,
    const uint32_t& a_SrcWidth, const uint32_t& a_SrcHeight, const uint32_t& a_SrcDepth,
    const Image& a_Src) const
{
    auto& pxDsc = a_Src.GetPixelDescriptor();
    OGLTextureUploadInfo info;
    info.level   = a_Level;
    info.offsetX = a_SrcOffsetX;
    info.offsetY = a_SrcOffsetY;
    info.offsetZ = a_SrcOffsetZ;
    info.width   = a_SrcWidth;
    info.height  = a_SrcHeight;
    info.depth   = a_SrcDepth;
    if (pxDsc.IsCompressed()) {
        info.format = GL_RGBA_DXT5_S3TC;
    } else {
        info.format = ToGL(a_Src.GetPixelDescriptor().GetUnsizedFormat());
        info.type   = ToGL(a_Src.GetPixelDescriptor().GetDataType());
    }
    glm::uvec3 srcOffset = { a_SrcOffsetX, a_SrcOffsetY, a_SrcOffsetZ };
    glm::uvec3 srcSize   = { a_SrcWidth, a_SrcHeight, a_SrcDepth };
    return UploadLevel(info, a_Src.Read(srcOffset, srcSize));
}

void OGLTexture::UploadLevel(const OGLTextureUploadInfo& a_Info, std::vector<std::byte> a_Data) const
{
    switch (target) {
    case GL_TEXTURE_1D:
    case GL_PROXY_TEXTURE_1D:
        ExecuteOGLCommand(context,
            [handle = handle, info = a_Info, data = std::move(a_Data)] {
                if (info.format == GL_RGBA_DXT5_S3TC) {
                    glCompressedTextureSubImage1D(
                        handle,
                        info.level,
                        info.offsetX,
                        info.width,
                        info.format,
                        GLsizei(data.size()),
                        data.data());
                } else {
                    glTextureSubImage1D(
                        handle,
                        info.level,
                        info.offsetX,
                        info.width,
                        info.format, info.type,
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
                if (info.format == GL_RGBA_DXT5_S3TC) {
                    glCompressedTextureSubImage2D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY,
                        info.width, info.height,
                        info.format,
                        GLsizei(data.size()),
                        data.data());
                } else {
                    glTextureSubImage2D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY,
                        info.width, info.height,
                        info.format, info.type,
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
                if (info.format == GL_RGBA_DXT5_S3TC) {
                    glCompressedTextureSubImage3D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY, info.offsetZ,
                        info.width, info.height, info.depth,
                        info.format,
                        GLsizei(data.size()),
                        data.data());
                } else {
                    glTextureSubImage3D(
                        handle,
                        info.level,
                        info.offsetX, info.offsetY, info.offsetZ,
                        info.width, info.height, info.depth,
                        info.format, info.type,
                        data.data());
                }
            });
        break;
    default:
        break;
    }
}

void Msg::OGLTexture::UploadLevel(const OGLTextureUploadInfo& a_Info, void* a_Data) const
{
    size_t dataSize    = GetPixelBufferSize(a_Info.format, a_Info.type, { a_Info.width, a_Info.height, a_Info.depth });
    std::byte* dataBeg = static_cast<std::byte*>(a_Data);
    std::byte* dataEnd = static_cast<std::byte*>(a_Data) + dataSize;
    UploadLevel(a_Info, std::vector<std::byte>(dataBeg, dataEnd));
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
