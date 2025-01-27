#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Image2D.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <utility>

namespace MSG {
OGLTexture2D::OGLTexture2D(
    OGLContext& a_Context,
    const unsigned& a_Width,
    const unsigned& a_Height,
    const unsigned& a_Levels,
    const unsigned& a_SizedFormat)
    : OGLTexture(a_Context, GL_TEXTURE_2D)
    , width(a_Width)
    , height(a_Height)
    , levels(a_Levels)
    , sizedFormat(a_SizedFormat)
{
    if (context.IsContextThread())
        glTextureStorage2D(handle, levels, sizedFormat, width, height);
    else
        context.PushCmd([=] {
            glTextureStorage2D(handle, levels, sizedFormat, width, height);
        });
}

void OGLTexture2D::UploadLevel(
    const unsigned& a_Level,
    const Image2D& a_Src) const
{
    const auto& SGImagePD       = a_Src.GetPixelDescriptor();
    const auto& SGImageAccessor = a_Src.GetBufferAccessor();
    const auto offset           = glm::ivec2 { 0, 0 };
    const auto size             = glm::ivec2 { a_Src.GetSize().x, a_Src.GetSize().y };
    if (SGImagePD.GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        if (context.IsContextThread())
            glCompressedTextureSubImage2D(
                handle,
                a_Level,
                offset.x, offset.y,
                size.x, size.y,
                sizedFormat,
                GLsizei(SGImageAccessor.GetByteLength()),
                std::to_address(SGImageAccessor.begin()));
        else
            context.PushCmd([=] {
                glCompressedTextureSubImage2D(
                    handle,
                    a_Level,
                    offset.x, offset.y,
                    size.x, size.y,
                    sizedFormat,
                    GLsizei(SGImageAccessor.GetByteLength()),
                    std::to_address(SGImageAccessor.begin()));
            });
    } else {
        const auto dataFormat = ToGL(SGImagePD.GetUnsizedFormat());
        const auto dataType   = ToGL(SGImagePD.GetDataType());
        if (context.IsContextThread())
            glTextureSubImage2D(
                handle,
                a_Level,
                offset.x, offset.y,
                size.x, size.y,
                dataFormat, dataType,
                std::to_address(SGImageAccessor.begin()));
        else
            context.PushCmd([=] {
                glTextureSubImage2D(
                    handle,
                    a_Level,
                    offset.x, offset.y,
                    size.x, size.y,
                    dataFormat, dataType,
                    std::to_address(SGImageAccessor.begin()));
            });
    }
}
}