#include <MSG/Image.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/Renderer/OGL/Loader/TextureLoader.hpp>
#include <MSG/Renderer/OGL/ToGL.hpp>
#include <MSG/Texture.hpp>
#include <MSG/Tools/LazyConstructor.hpp>

#include <GL/glew.h>
#include <array>

namespace Msg::Renderer {
static auto LoadTexture2D(OGLContext& a_Context, Texture& a_Texture, const bool& a_Sparse)
{
    auto const& SGImagePD   = a_Texture.GetPixelDescriptor();
    auto const& SGImageSize = a_Texture.GetSize();
    uint32_t sizedFormat    = ToGL(SGImagePD.GetSizedFormat());
    uint32_t levelCount     = a_Texture.size();
    OGLTexture2DInfo info   = {
          .width       = SGImageSize.x,
          .height      = SGImageSize.y,
          .levels      = levelCount,
          .sizedFormat = sizedFormat,
          .sparse      = a_Sparse
    };
    auto texture = std::make_shared<OGLTexture2D>(a_Context, info);
    if (!a_Sparse) {
        // upload data if texture is not sparse, otherwise it'll be uploaded later
        a_Context.PushCmd(
            [texture, levels = a_Texture] {
                for (auto level = 0; level < levels.size(); level++)
                    texture->UploadLevel(level, *levels.at(level));
            });
    }
    return std::static_pointer_cast<OGLTexture>(texture);
}

static auto LoadTextureCubemap(OGLContext& a_Context, Texture& a_Texture, const bool& a_Sparse)
{
    auto const& SGImagePD   = a_Texture.GetPixelDescriptor();
    auto const& SGImageSize = a_Texture.GetSize();
    uint32_t sizedFormat    = ToGL(SGImagePD.GetSizedFormat());
    uint32_t levelCount     = a_Texture.size();
    OGLTextureCubeInfo info = {
        .width       = SGImageSize.x,
        .height      = SGImageSize.y,
        .levels      = levelCount,
        .sizedFormat = sizedFormat,
        .sparse      = a_Sparse
    };
    auto texture = std::make_shared<OGLTextureCube>(a_Context, info);
    if (!a_Sparse) {
        a_Context.PushCmd(
            [texture, levels = a_Texture] {
                for (auto level = 0; level < levels.size(); level++)
                    texture->UploadLevel(level, *levels.at(level));
            });
    }
    return std::static_pointer_cast<OGLTexture>(texture);
}

// TODO Find a better way to do this
std::shared_ptr<OGLTexture> TextureLoader::operator()(OGLContext& a_Context, Texture* a_Texture, const bool& a_Sparse)
{
    if (a_Texture->GetType() == TextureType::Texture1D) {
        // texture1DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture1D(context, image);
        // });
    } else if (a_Texture->GetType() == TextureType::Texture2D) {
        auto textureFactory = Tools::LazyConstructor([&a_Context, &a_Texture, &a_Sparse] {
            return LoadTexture2D(a_Context, *a_Texture, a_Sparse);
        });
        return textureCache.GetOrCreate(a_Texture, textureFactory);
    } else if (a_Texture->GetType() == TextureType::Texture3D) {
        // texture3DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture3D(context, image);
        // });
    } else if (a_Texture->GetType() == TextureType::TextureCubemap) {
        auto textureFactory = Tools::LazyConstructor([&a_Context, &a_Texture, &a_Sparse] {
            return LoadTextureCubemap(a_Context, *a_Texture, a_Sparse);
        });
        return textureCache.GetOrCreate(a_Texture, textureFactory);
    }
    return nullptr;
}
}
