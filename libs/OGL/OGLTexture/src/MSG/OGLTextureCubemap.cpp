#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTextureCubemap.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <utility>

namespace MSG {
OGLTextureCubemap::OGLTextureCubemap(
    OGLContext& a_Context,
    const unsigned& a_Width,
    const unsigned& a_Height,
    const unsigned& a_Levels,
    const unsigned& a_SizedFormat)
    : OGLTexture(a_Context, GL_TEXTURE_CUBE_MAP)
    , width(a_Width)
    , height(a_Height)
    , levels(a_Levels)
    , sizedFormat(a_SizedFormat)
{
    ExecuteOGLCommand(context, [handle = handle, levels = levels, sizedFormat = sizedFormat, width = width, height = height] {
        glTextureStorage2D(handle, levels, sizedFormat, width, height);
    });
}

void OGLTextureCubemap::UploadLevel(
    const unsigned& a_Level,
    const Image& a_Src) const
{
    const auto& SGImagePD       = a_Src.GetPixelDescriptor();
    const auto& SGImageAccessor = a_Src.GetBufferAccessor();
    const auto offset           = glm::ivec3 { 0, 0, 0 };
    const auto size             = glm::ivec3 { a_Src.GetSize().x, a_Src.GetSize().y, a_Src.GetSize().z };
    if (SGImagePD.GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        ExecuteOGLCommand(context, [handle = handle, level = a_Level, sizedFormat = sizedFormat, offset, size, SGImageAccessor] {
            glCompressedTextureSubImage3D(
                handle,
                level,
                offset.x, offset.y, offset.z,
                size.x, size.y, size.z,
                sizedFormat,
                GLsizei(SGImageAccessor.GetByteLength()),
                std::to_address(SGImageAccessor.begin()));
        });
    } else {
        const auto dataFormat = ToGL(SGImagePD.GetUnsizedFormat());
        const auto dataType   = ToGL(SGImagePD.GetDataType());
        ExecuteOGLCommand(context, [handle = handle, level = a_Level, dataFormat, dataType, offset, size, SGImageAccessor] {
            glTextureSubImage3D(
                handle,
                level,
                offset.x, offset.y, offset.z,
                size.x, size.y, size.z,
                dataFormat, dataType,
                std::to_address(SGImageAccessor.begin()));
        });
    }
}
}