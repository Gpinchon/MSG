#include <Renderer/OGL/Loader/TextureLoader.hpp>
#include <Renderer/OGL/RAII/Texture.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/ToGL.hpp>

#include <SG/Core/Buffer/View.hpp>
#include <SG/Core/Image/Cubemap.hpp>
#include <SG/Core/Image/Image2D.hpp>
#include <SG/Core/Texture/Texture.hpp>
#include <Tools/LazyConstructor.hpp>

#include <GL/glew.h>
#include <array>

namespace TabGraph::Renderer {
static auto LoadTexture2D(Context& a_Context, SG::Texture& a_Texture)
{
    auto const& SGImagePD   = a_Texture.GetPixelDescription();
    auto const& SGImageSize = a_Texture.GetSize();
    auto sizedFormat        = ToGL(SGImagePD.GetSizedFormat());
    auto levelCount         = a_Texture.size();
    auto texture            = RAII::MakePtr<RAII::Texture2D>(a_Context,
                   SGImageSize.x, SGImageSize.y, levelCount, sizedFormat);
    a_Context.PushCmd(
        [texture, levels = a_Texture] {
            for (auto level = 0; level < levels.size(); level++)
                texture->UploadLevel(level, *std::static_pointer_cast<SG::Image2D>(levels.at(level)));
        });
    return std::static_pointer_cast<RAII::Texture>(texture);
}

static auto LoadTextureCubemap(Context& a_Context, SG::Texture& a_Texture)
{
    auto const& SGImagePD   = a_Texture.GetPixelDescription();
    auto const& SGImageSize = a_Texture.GetSize();
    auto sizedFormat        = ToGL(SGImagePD.GetSizedFormat());
    auto levelCount         = a_Texture.size();
    auto texture            = RAII::MakePtr<RAII::TextureCubemap>(a_Context,
                   SGImageSize.x, SGImageSize.y, levelCount, sizedFormat);
    a_Context.PushCmd(
        [texture, levels = a_Texture] {
            for (auto level = 0; level < levels.size(); level++)
                texture->UploadLevel(level, *std::static_pointer_cast<SG::Cubemap>(levels.at(level)));
        });
    return std::static_pointer_cast<RAII::Texture>(texture);
}

std::shared_ptr<RAII::Texture> TextureLoader::operator()(Context& a_Context, SG::Texture* a_Texture)
{
    if (a_Texture->GetType() == SG::TextureType::Texture1D) {
        // texture1DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture1D(context, image);
        // });
    } else if (a_Texture->GetType() == SG::TextureType::Texture2D) {
        auto textureFactory = Tools::LazyConstructor([&context = a_Context, sgTexture = a_Texture] {
            return LoadTexture2D(context, *sgTexture);
        });
        return textureCache.GetOrCreate(a_Texture, textureFactory);
    } else if (a_Texture->GetType() == SG::TextureType::Texture3D) {
        // texture3DCache.GetOrCreate(a_Image, [&context = context, image = a_Image] {
        //     return LoadTexture3D(context, image);
        // });
    } else if (a_Texture->GetType() == SG::TextureType::TextureCubemap) {
        auto textureFactory = Tools::LazyConstructor([&context = a_Context, sgTexture = a_Texture] {
            return LoadTextureCubemap(context, *sgTexture);
        });
        return textureCache.GetOrCreate(a_Texture, textureFactory);
    }
    return nullptr;
}
}
