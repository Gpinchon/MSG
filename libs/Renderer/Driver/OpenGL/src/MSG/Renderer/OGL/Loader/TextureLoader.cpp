#include <MSG/Buffer/View.hpp>
#include <MSG/Cubemap.hpp>
#include <MSG/Image2D.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTextureCubemap.hpp>
#include <MSG/Renderer/OGL/Loader/TextureLoader.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

#include <GL/glew.h>
#include <array>

namespace MSG::Renderer {
static auto LoadTexture2D(OGLContext& a_Context, Texture& a_Texture)
{
    auto const& SGImagePD   = a_Texture.GetPixelDescriptor();
    auto const& SGImageSize = a_Texture.GetSize();
    auto sizedFormat        = ToGL(SGImagePD.GetSizedFormat());
    auto levelCount         = a_Texture.size();
    auto texture            = std::make_shared<OGLTexture2D>(a_Context, SGImageSize.x, SGImageSize.y, levelCount, sizedFormat);
    a_Context.PushCmd(
        [texture, levels = a_Texture] {
            for (auto level = 0; level < levels.size(); level++)
                texture->UploadLevel(level, *std::static_pointer_cast<Image2D>(levels.at(level)));
        });
    return std::static_pointer_cast<OGLTexture>(texture);
}

static auto LoadTextureCubemap(OGLContext& a_Context, Texture& a_Texture)
{
    auto const& SGImagePD   = a_Texture.GetPixelDescriptor();
    auto const& SGImageSize = a_Texture.GetSize();
    auto sizedFormat        = ToGL(SGImagePD.GetSizedFormat());
    auto levelCount         = a_Texture.size();
    auto texture            = std::make_shared<OGLTextureCubemap>(a_Context, SGImageSize.x, SGImageSize.y, levelCount, sizedFormat);
    a_Context.PushCmd(
        [texture, levels = a_Texture] {
            for (auto level = 0; level < levels.size(); level++)
                texture->UploadLevel(level, *std::static_pointer_cast<Cubemap>(levels.at(level)));
        });
    return std::static_pointer_cast<OGLTexture>(texture);
}

std::shared_ptr<OGLTexture> TextureLoader::operator()(OGLContext& a_Context, Texture* a_Texture)
{
    if (a_Texture->GetType() == TextureType::Texture1D) {
        // texture1DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture1D(context, image);
        // });
    } else if (a_Texture->GetType() == TextureType::Texture2D) {
        auto textureFactory = Tools::LazyConstructor([&context = a_Context, sgTexture = a_Texture] {
            return LoadTexture2D(context, *sgTexture);
        });
        return textureCache.GetOrCreate(a_Texture, textureFactory);
    } else if (a_Texture->GetType() == TextureType::Texture3D) {
        // texture3DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture3D(context, image);
        // });
    } else if (a_Texture->GetType() == TextureType::TextureCubemap) {
        auto textureFactory = Tools::LazyConstructor([&context = a_Context, sgTexture = a_Texture] {
            return LoadTextureCubemap(context, *sgTexture);
        });
        return textureCache.GetOrCreate(a_Texture, textureFactory);
    }
    return nullptr;
}
}
