#include <Renderer/OGL/Loader/TextureLoader.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <Tools/LazyConstructor.hpp>

#include <GL/glew.h>
#include <array>

namespace TabGraph::Renderer {
static inline auto ToGL(const SG::Pixel::Type& a_Type)
{
    switch (a_Type) {
    case SG::Pixel::Type::Uint8:
        return GL_UNSIGNED_BYTE;
    case SG::Pixel::Type::Int8:
        return GL_BYTE;
    case SG::Pixel::Type::Uint16:
        return GL_UNSIGNED_SHORT;
    case SG::Pixel::Type::Int16:
        return GL_SHORT;
    case SG::Pixel::Type::Uint32:
        return GL_UNSIGNED_INT;
    case SG::Pixel::Type::Int32:
        return GL_INT;
    case SG::Pixel::Type::Float16:
        return GL_HALF_FLOAT;
    case SG::Pixel::Type::Float32:
        return GL_FLOAT;
    default:
        throw std::runtime_error("Unknown Pixel Type");
    }
    return GL_NONE;
}
static inline auto ToGL(const SG::Pixel::UnsizedFormat& a_UnsizedFormat)
{
    switch (a_UnsizedFormat) {
    case SG::Pixel::UnsizedFormat::R:
    case SG::Pixel::UnsizedFormat::R_Integer:
        return GL_R;
    case SG::Pixel::UnsizedFormat::RG:
    case SG::Pixel::UnsizedFormat::RG_Integer:
        return GL_RG;
    case SG::Pixel::UnsizedFormat::RGB:
    case SG::Pixel::UnsizedFormat::RGB_Integer:
        return GL_RGB;
    case SG::Pixel::UnsizedFormat::RGBA:
    case SG::Pixel::UnsizedFormat::RGBA_Integer:
        return GL_RGBA;
    case SG::Pixel::UnsizedFormat::Depth:
        return GL_DEPTH_COMPONENT;
    case SG::Pixel::UnsizedFormat::Depth_Stencil:
        return GL_DEPTH_STENCIL;
    case SG::Pixel::UnsizedFormat::Stencil:
        return GL_STENCIL_INDEX;
    default:
        throw std::runtime_error("Unknown Pixel Unsized Format");
    }
    return GL_NONE;
}
static inline auto ToGL(const SG::Pixel::SizedFormat& a_SizedFormat)
{
    switch (a_SizedFormat) {
    case SG::Pixel::SizedFormat::Uint8_NormalizedR:
        return GL_R8;
    case SG::Pixel::SizedFormat::Uint8_NormalizedRG:
        return GL_RG8;
    case SG::Pixel::SizedFormat::Uint8_NormalizedRGB:
        return GL_RGB8;
    case SG::Pixel::SizedFormat::Uint8_NormalizedRGBA:
        return GL_RGBA8;
    case SG::Pixel::SizedFormat::Int8_NormalizedR:
        return GL_RGB8_SNORM;
    case SG::Pixel::SizedFormat::Int8_NormalizedRG:
        return GL_RG8_SNORM;
    case SG::Pixel::SizedFormat::Int8_NormalizedRGB:
        return GL_RGB8_SNORM;
    case SG::Pixel::SizedFormat::Int8_NormalizedRGBA:
        return GL_RGBA8_SNORM;
    case SG::Pixel::SizedFormat::Uint8_R:
        return GL_R8UI;
    case SG::Pixel::SizedFormat::Uint8_RG:
        return GL_RG8UI;
    case SG::Pixel::SizedFormat::Uint8_RGB:
        return GL_RGB8UI;
    case SG::Pixel::SizedFormat::Uint8_RGBA:
        return GL_RGBA8UI;
    case SG::Pixel::SizedFormat::Int8_R:
        return GL_R8I;
    case SG::Pixel::SizedFormat::Int8_RG:
        return GL_RG8I;
    case SG::Pixel::SizedFormat::Int8_RGB:
        return GL_RGB8I;
    case SG::Pixel::SizedFormat::Int8_RGBA:
        return GL_RGBA8I;
    case SG::Pixel::SizedFormat::Uint16_NormalizedR:
        return GL_R16;
    case SG::Pixel::SizedFormat::Uint16_NormalizedRG:
        return GL_RG16;
    case SG::Pixel::SizedFormat::Uint16_NormalizedRGB:
        return GL_RGB16;
    case SG::Pixel::SizedFormat::Uint16_NormalizedRGBA:
        return GL_RGBA16;
    case SG::Pixel::SizedFormat::Int16_NormalizedR:
        return GL_R16_SNORM;
    case SG::Pixel::SizedFormat::Int16_NormalizedRG:
        return GL_RG16_SNORM;
    case SG::Pixel::SizedFormat::Int16_NormalizedRGB:
        return GL_RGB16_SNORM;
    case SG::Pixel::SizedFormat::Int16_NormalizedRGBA:
        return GL_RGBA16_SNORM;
    case SG::Pixel::SizedFormat::Uint16_R:
        return GL_R16UI;
    case SG::Pixel::SizedFormat::Uint16_RG:
        return GL_RG16UI;
    case SG::Pixel::SizedFormat::Uint16_RGB:
        return GL_RGB16UI;
    case SG::Pixel::SizedFormat::Uint16_RGBA:
        return GL_RGBA16UI;
    case SG::Pixel::SizedFormat::Int16_R:
        return GL_R16I;
    case SG::Pixel::SizedFormat::Int16_RG:
        return GL_RG16I;
    case SG::Pixel::SizedFormat::Int16_RGB:
        return GL_RGB16I;
    case SG::Pixel::SizedFormat::Int16_RGBA:
        return GL_RGBA16I;
    case SG::Pixel::SizedFormat::Uint32_R:
        return GL_R32UI;
    case SG::Pixel::SizedFormat::Uint32_RG:
        return GL_RG32UI;
    case SG::Pixel::SizedFormat::Uint32_RGB:
        return GL_RGB32UI;
    case SG::Pixel::SizedFormat::Uint32_RGBA:
        return GL_RGBA32UI;
    case SG::Pixel::SizedFormat::Int32_R:
        return GL_R32I;
    case SG::Pixel::SizedFormat::Int32_RG:
        return GL_RG32I;
    case SG::Pixel::SizedFormat::Int32_RGB:
        return GL_RGB32I;
    case SG::Pixel::SizedFormat::Int32_RGBA:
        return GL_RGBA32I;
    case SG::Pixel::SizedFormat::Float16_R:
        return GL_R16F;
    case SG::Pixel::SizedFormat::Float16_RG:
        return GL_RG16F;
    case SG::Pixel::SizedFormat::Float16_RGB:
        return GL_RGB16F;
    case SG::Pixel::SizedFormat::Float16_RGBA:
        return GL_RGBA16F;
    case SG::Pixel::SizedFormat::Float32_R:
        return GL_R32F;
    case SG::Pixel::SizedFormat::Float32_RG:
        return GL_RG32F;
    case SG::Pixel::SizedFormat::Float32_RGB:
        return GL_RGB32F;
    case SG::Pixel::SizedFormat::Float32_RGBA:
        return GL_RGBA32F;
    case SG::Pixel::SizedFormat::Depth16:
        return GL_DEPTH_COMPONENT16;
    case SG::Pixel::SizedFormat::Depth24:
        return GL_DEPTH_COMPONENT24;
    case SG::Pixel::SizedFormat::Depth32:
        return GL_DEPTH_COMPONENT32;
    case SG::Pixel::SizedFormat::Depth32F:
        return GL_DEPTH_COMPONENT32F;
    case SG::Pixel::SizedFormat::Depth24_Stencil8:
        return GL_DEPTH24_STENCIL8;
    case SG::Pixel::SizedFormat::Depth32F_Stencil8:
        return GL_DEPTH32F_STENCIL8;
    case SG::Pixel::SizedFormat::Stencil8:
        return GL_STENCIL_INDEX8;
    case SG::Pixel::SizedFormat::DXT5_RGBA:
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    default:
        throw std::runtime_error("Unknown Pixel Sized Format");
    }
    return GL_NONE;
}

auto LoadTexture2D(Context& a_Context, SG::Image* a_Image)
{
    auto& SGImagePD   = a_Image->GetPixelDescription();
    auto& SGImageSize = a_Image->GetSize();
    auto sizedFormat  = ToGL(SGImagePD.GetSizedFormat());
    auto texture      = RAII::MakePtr<RAII::Texture2D>(a_Context,
             SGImageSize.x, SGImageSize.y, SGImageSize.z, sizedFormat);
    if (SGImagePD.GetSizedFormat() == SG::Pixel::SizedFormat::DXT5_RGBA) {
        a_Context.PushCmd(
            [texture,
                textureSizedFormat = ToGL(SGImagePD.GetSizedFormat()),
                SGImageBV          = a_Image->GetBufferView()]() {
                glCompressedTextureSubImage2D(
                    *texture,
                    0, 0, 0,
                    texture->width, texture->height,
                    textureSizedFormat,
                    SGImageBV->GetByteLength(),
                    SGImageBV->begin());
            });
    } else {
        a_Context.PushCmd(
            [texture,
                textureDataFormat = ToGL(SGImagePD.GetUnsizedFormat()),
                textureDataType   = ToGL(SGImagePD.GetType()),
                SGImageBV         = a_Image->GetBufferView()]() {
                glTextureSubImage2D(*texture,
                    0, 0, 0,
                    texture->width, texture->height,
                    textureDataFormat, textureDataType, SGImageBV->begin());
            });
    }
    return texture;
}

std::shared_ptr<RAII::Texture> TextureLoader::operator()(Context& a_Context, SG::Image* a_Image)
{
    if (a_Image->GetType() == SG::Image::Type::Image1D) {
        // texture1DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture1D(context, image);
        // });
    } else if (a_Image->GetType() == SG::Image::Type::Image2D) {
        auto texture2DFactory = Tools::LazyConstructor([&context = a_Context, image = a_Image] {
            return LoadTexture2D(context, image);
        });
        return texture2DCache.GetOrCreate(a_Image, texture2DFactory);
    } else if (a_Image->GetType() == SG::Image::Type::Image3D) {
        // texture3DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture3D(context, image);
        // });
    }
    return nullptr;
}
}
