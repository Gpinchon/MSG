#include <MSG/Buffer/Accessor.hpp>
#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture3D.hpp>
#include <MSG/ToGL.hpp>

#include <GL/glew.h>
#include <glm/vec2.hpp>
#include <utility>

namespace MSG {
OGLTextureInfo GetTextureInfo(const OGLTexture3DInfo& a_Info)
{
    return {
        .target      = GL_TEXTURE_3D,
        .width       = a_Info.width,
        .height      = a_Info.height,
        .depth       = a_Info.depth,
        .levels      = a_Info.levels,
        .sizedFormat = a_Info.sizedFormat,
    };
}
OGLTexture3D::OGLTexture3D(
    OGLContext& a_Context,
    const OGLTexture3DInfo& a_Info)
    : OGLTexture(a_Context, GetTextureInfo(a_Info))
{
    ExecuteOGLCommand(context, [handle = handle, levels = levels, sizedFormat = sizedFormat, width = width, height = height, depth = depth] {
        glTextureStorage3D(handle, levels, sizedFormat, width, height, depth);
    });
}

void OGLTexture3D::UploadLevel(
    const unsigned& a_Level,
    const Image& a_Src) const
{
    const auto& SGImagePD       = a_Src.GetPixelDescriptor();
    const auto& SGImageAccessor = a_Src.GetBufferAccessor();
    const auto offset           = glm::ivec3 { 0, 0, 0 };
    const auto size             = glm::ivec3 { a_Src.GetSize().x, a_Src.GetSize().y, a_Src.GetSize().z };
    if (SGImagePD.GetSizedFormat() == PixelSizedFormat::DXT5_RGBA) {
        ExecuteOGLCommand(context, [handle = handle, level = a_Level, offset = offset, size = size, sizedFormat = sizedFormat, SGImageAccessor = SGImageAccessor] {
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
        ExecuteOGLCommand(context, [handle = handle, level = a_Level, offset = offset, size = size, dataFormat = dataFormat, dataType = dataType, SGImageAccessor = SGImageAccessor] {
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