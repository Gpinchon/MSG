#include <Renderer/OGL/GLEnum.hpp>
#include <Renderer/OGL/Loader/TextureLoader.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>

#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Image.hpp>
#include <Tools/LazyConstructor.hpp>

#include <GL/glew.h>
#include <array>

namespace TabGraph::Renderer {
auto LoadTexture2D(Context& a_Context, SG::Image* a_Image, const uint& a_Mips)
{
    auto& SGImagePD   = a_Image->GetPixelDescription();
    auto& SGImageSize = a_Image->GetSize();
    auto sizedFormat  = ToGL(SGImagePD.GetSizedFormat());
    auto texture      = RAII::MakePtr<RAII::Texture2D>(a_Context,
             SGImageSize.x, SGImageSize.y, a_Mips, sizedFormat);
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

auto LoadTextureCubemap(Context& a_Context, SG::Image* a_Image, const uint& a_Mips)
{
    auto const& SGImagePD   = a_Image->GetPixelDescription();
    auto const& SGImageSize = a_Image->GetSize();
    auto sizedFormat        = ToGL(SGImagePD.GetSizedFormat());
    auto texture            = RAII::MakePtr<RAII::TextureCubemap>(a_Context,
                   SGImageSize.x, SGImageSize.y, a_Mips, sizedFormat);
    if (SGImagePD.GetSizedFormat() == SG::Pixel::SizedFormat::DXT5_RGBA) {
        a_Context.PushCmd(
            [texture,
                textureSizedFormat = ToGL(SGImagePD.GetSizedFormat()),
                SGImageBV          = a_Image->GetBufferView()]() {
                glCompressedTextureSubImage3D(
                    *texture,
                    0,
                    0, 0, 0,
                    texture->width, texture->height, 6,
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
                glTextureSubImage3D(*texture,
                    0,
                    0, 0, 0,
                    texture->width, texture->height, 6,
                    textureDataFormat, textureDataType, SGImageBV->begin());
            });
    }
    return texture;
}

std::shared_ptr<RAII::Texture> TextureLoader::operator()(Context& a_Context, SG::Image* a_Image, const uint& a_Mips)
{
    if (a_Image->GetType() == SG::ImageType::Image1D) {
        // texture1DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture1D(context, image);
        // });
    } else if (a_Image->GetType() == SG::ImageType::Image2D) {
        auto textureFactory = Tools::LazyConstructor([&context = a_Context, image = a_Image, mips = a_Mips] {
            auto texture = LoadTexture2D(context, image, mips);
            glGenerateTextureMipmap(*texture);
            return texture;
        });
        return texture2DCache.GetOrCreate(a_Image, textureFactory);
    } else if (a_Image->GetType() == SG::ImageType::Image3D) {
        // texture3DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture3D(context, image);
        // });
    } else if (a_Image->GetType() == SG::ImageType::Cubemap) {
        auto textureFactory = Tools::LazyConstructor([&context = a_Context, image = a_Image, mips = a_Mips] {
            auto texture = LoadTextureCubemap(context, image, mips);
            glGenerateTextureMipmap(*texture);
            return texture;
        });
        return textureCubemapCache.GetOrCreate(a_Image, textureFactory);
    }
    return nullptr;
}
}
