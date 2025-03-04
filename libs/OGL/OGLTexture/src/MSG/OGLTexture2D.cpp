#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <utility>

namespace MSG {
OGLTexture2D::OGLTexture2D(
    OGLContext& a_Context,
    const unsigned& a_Width,
    const unsigned& a_Height,
    const unsigned& a_Levels,
    const unsigned& a_SizedFormat)
    : OGLTexture(
          a_Context, GL_TEXTURE_2D, a_SizedFormat,
          a_Width, a_Height, 1,
          a_Levels)
{
    ExecuteOGLCommand(context, [handle = handle, levels = levels, sizedFormat = sizedFormat, width = width, height = height] {
        glTextureStorage2D(handle, levels, sizedFormat, width, height);
    });
}

void OGLTexture2D::UploadLevel(
    const unsigned& a_Level,
    const Image& a_Src) const
{
    const auto& SGImagePD       = a_Src.GetPixelDescriptor();
    const auto& SGImageAccessor = a_Src.GetBufferAccessor();
    const auto offset           = glm::ivec2 { 0, 0 };
    const auto size             = glm::ivec2 { a_Src.GetSize().x, a_Src.GetSize().y };
    if (SGImagePD.GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        ExecuteOGLCommand(context, [handle = handle, level = a_Level, offset = offset, size = size, sizedFormat = sizedFormat, SGImageAccessor = SGImageAccessor] {
            glCompressedTextureSubImage2D(
                handle,
                level,
                offset.x, offset.y,
                size.x, size.y,
                sizedFormat,
                GLsizei(SGImageAccessor.GetByteLength()),
                std::to_address(SGImageAccessor.begin()));
        });
    } else {
        const auto dataFormat = ToGL(SGImagePD.GetUnsizedFormat());
        const auto dataType   = ToGL(SGImagePD.GetDataType());
        ExecuteOGLCommand(context, [handle = handle, level = a_Level, offset = offset, size = size, dataFormat = dataFormat, dataType = dataType, SGImageAccessor = SGImageAccessor] {
            glTextureSubImage2D(
                handle,
                level,
                offset.x, offset.y,
                size.x, size.y,
                dataFormat, dataType,
                std::to_address(SGImageAccessor.begin()));
        });
    }
}
}