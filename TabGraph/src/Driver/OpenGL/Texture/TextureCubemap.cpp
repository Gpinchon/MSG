/*
* @Author: gpinchon
* @Date:   2021-05-04 17:11:15
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:23
*/

#include <Driver/OpenGL/Texture/TextureCubemap.hpp>
#include <Driver/OpenGL/Texture/PixelUtils.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Parser.hpp>
#include <Assets/Image.hpp>

#include <GL/glew.h>

namespace TabGraph::Textures {
TextureCubemap::Impl::Impl(const Impl& other)
    : Texture::Impl(other)
    , _size(other._size)
    , _asset(other._asset)
{
    _type = Texture::Type::TextureCubemap;
}

TextureCubemap::Impl::Impl(std::shared_ptr<Assets::Asset> image)
    : Texture::Impl(Texture::Type::TextureCubemap)
    , _asset(image)
{
}

TextureCubemap::Impl::Impl(const glm::ivec2& size, const Pixel::SizedFormat& format)
    : Texture::Impl(Texture::Type::TextureCubemap, format)
{
    SetSize(size);
}

void TextureCubemap::Impl::Load()
{
    if (GetLoaded())
        return;
    if (GetImage() != nullptr) {
        Assets::Parser::AddParsingTask({
            Assets::Parser::ParsingTask::Type::Sync,
            GetImage()
        });
        auto image { GetImage()->Get<Assets::Image>().at(0) }; //will crash if no image
        SetSize(glm::ivec2(std::min(image->GetSize().x, image->GetSize().y)));
        SetPixelDescription(image->GetPixelDescription());
        _AllocateStorage();
        std::array<std::shared_ptr<Assets::Image>, 6> sides;
        std::array<std::thread, 6> loadingThreads;
        for (auto sideIndex = 0; sideIndex < 6; ++sideIndex) {
            sides.at(sideIndex) = std::make_shared<Assets::Image>(GetSize(), GetPixelDescription());
            loadingThreads.at(sideIndex) = std::thread(ExtractSide, image, sides.at(sideIndex), (TextureCubemap::Side)sideIndex);
        }
        for (auto sideIndex = 0; sideIndex < 6; ++sideIndex) {
            loadingThreads.at(sideIndex).join();
            auto side { sides.at(sideIndex) };
            glTextureSubImage3D(
                GetHandle(),
                0,
                0,
                0,
                sideIndex,
                side->GetSize().x,
                side->GetSize().y,
                1,
                OpenGL::GetEnum(side->GetPixelDescription().GetUnsizedFormat()),
                OpenGL::GetEnum(side->GetPixelDescription().GetType()),
                side->GetData().data());
        }
        SetImage(nullptr);
    }
    else
        _AllocateStorage();
    if (GetAutoMipMap()) GenerateMipmap();
    SetLoaded(true);
}

void TextureCubemap::Impl::SetImage(std::shared_ptr<Assets::Asset> image)
{
    if (image == GetImage()) return;
    _asset = image;
    Unload();
}

void TextureCubemap::Impl::SetSize(const glm::ivec2 size)
{
    if (size == GetSize()) return;
    _size = size;
    Unload();
    if (GetAutoMipMap()) SetMipMapNbr(MIPMAPNBR(GetSize()));
}

std::shared_ptr<Assets::Asset> TextureCubemap::Impl::GetImage() const
{
    return _asset;
}

glm::ivec2 TextureCubemap::Impl::GetSize() const
{
    return _size;
}

void TextureCubemap::Impl::Unload()
{
    if (!GetLoaded())
        return;
    OpenGL::Texture::Delete(GetHandle());
    _handle = 0;
    SetLoaded(false);
}

void TextureCubemap::Impl::GenerateMipmap()
{
    Bind();
    glGenerateMipmap(OpenGL::GetEnum(Texture::Type::TextureCubemap));
    Done();
}

void TextureCubemap::Impl::_AllocateStorage()
{
    _handle = OpenGL::Texture::Generate();
    Bind();
    glTexStorage2D(
        OpenGL::GetEnum(Texture::Type::TextureCubemap),
        GetMipMapNbr(),
        OpenGL::GetEnum(GetPixelDescription().GetSizedFormat()),
        GetSize().x, GetSize().y
    );
    Done();
}
}
