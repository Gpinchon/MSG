#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/ToGL.hpp>

#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Image/Image2D.hpp>

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace TabGraph::Renderer::RAII {
static inline auto CreateTexture(const GLenum& a_Target)
{
    GLuint handle = 0;
    glCreateTextures(a_Target, 1, &handle);
    return handle;
}

Texture::Texture(const unsigned& a_Target)
    : target(a_Target)
    , handle(CreateTexture(a_Target))
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &handle);
}

Texture2D::Texture2D(
    const unsigned& a_Width,
    const unsigned& a_Height,
    const unsigned& a_Levels,
    const unsigned& a_SizedFormat)
    : Texture(GL_TEXTURE_2D)
    , width(a_Width)
    , height(a_Height)
    , levels(a_Levels)
    , sizedFormat(a_SizedFormat)
{
    glTextureStorage2D(handle, a_Levels, sizedFormat, a_Width, a_Height);
}

void Texture2D::UploadLevel(
    const unsigned& a_Level,
    const SG::Image2D& a_Src) const
{
    const auto& SGImagePD       = a_Src.GetPixelDescription();
    const auto& SGImageAccessor = a_Src.GetBufferAccessor();
    const auto offset           = glm::ivec2 { 0, 0 };
    const auto size             = glm::ivec2 { a_Src.GetSize().x, a_Src.GetSize().y };
    if (SGImagePD.GetSizedFormat() == SG::Pixel::SizedFormat::DXT5_RGBA) {
        glCompressedTextureSubImage2D(
            handle,
            0, 0, 0,
            width, height,
            sizedFormat,
            GLsizei(SGImageAccessor.GetByteLength()),
            &*SGImageAccessor.begin());
    } else {
        const auto dataFormat = ToGL(SGImagePD.GetUnsizedFormat());
        const auto dataType   = ToGL(SGImagePD.GetDataType());
        glTextureSubImage2D(
            handle,
            a_Level,
            offset.x, offset.y,
            size.x, size.y,
            dataFormat, dataType, &*a_Src.GetBufferAccessor().begin());
    }
}

TextureCubemap::TextureCubemap(
    const unsigned& a_Width,
    const unsigned& a_Height,
    const unsigned& a_Levels,
    const unsigned& a_SizedFormat)
    : Texture(GL_TEXTURE_CUBE_MAP)
    , width(a_Width)
    , height(a_Height)
    , levels(a_Levels)
    , sizedFormat(a_SizedFormat)
{
    glTextureStorage2D(handle, a_Levels, sizedFormat, a_Width, a_Height);
}

void TextureCubemap::UploadLevel(
    const unsigned& a_Level,
    const SG::Cubemap& a_Src) const
{
    const auto& SGImagePD       = a_Src.GetPixelDescription();
    const auto& SGImageAccessor = a_Src.GetBufferAccessor();
    const auto offset           = glm::ivec3 { 0, 0, 0 };
    const auto size             = glm::ivec3 { a_Src.GetSize().x, a_Src.GetSize().y, a_Src.GetSize().z };
    if (SGImagePD.GetSizedFormat() == SG::Pixel::SizedFormat::DXT5_RGBA) {
        glCompressedTextureSubImage3D(
            handle,
            0,
            0, 0, 0,
            width, height, 6,
            sizedFormat,
            GLsizei(SGImageAccessor.GetByteLength()),
            &*SGImageAccessor.begin());
    } else {
        const auto dataFormat = ToGL(SGImagePD.GetUnsizedFormat());
        const auto dataType   = ToGL(SGImagePD.GetDataType());
        glTextureSubImage3D(
            handle,
            a_Level,
            offset.x, offset.y, offset.z,
            size.x, size.y, size.z,
            dataFormat, dataType, &*a_Src.GetBufferAccessor().begin());
    }
}
}
