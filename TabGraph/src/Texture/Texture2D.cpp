#include <Texture/Texture2D.hpp>
#include <Texture/PixelUtils.hpp>
#include <Assets/Asset.hpp>

#if RENDERINGAPI == OpenGL
#include "Driver/OpenGL/Texture/Texture2D.hpp"
#endif

using namespace TabGraph::Textures;

Texture2D::Texture2D(const Texture2D& other)
    : Texture(other)
{
    _impl.reset(new Texture2D::Impl(static_cast<const Texture2D::Impl&>(*other._impl.get())));
}

Texture2D::Texture2D(const glm::ivec2& size, const Pixel::Description& pixelDesc)
    : Texture()
{
    _impl.reset(new Texture2D::Impl(size, pixelDesc));
}

Texture2D::Texture2D(std::shared_ptr<Assets::Asset> image)
    : Texture()
{
    _impl.reset(new Texture2D::Impl(image));
}

void Texture2D::SetSize(const glm::ivec2& size)
{
    return static_cast<Texture2D::Impl*>(_impl.get())->SetSize(size);
}

glm::ivec2 Texture2D::GetSize() const
{
    return static_cast<Texture2D::Impl*>(_impl.get())->GetSize();
}

void Texture2D::SetCompressionQuality(float compression)
{
    return static_cast<Texture2D::Impl*>(_impl.get())->SetCompressionQuality(compression);
}

float Texture2D::GetCompressionQuality() const
{
    return static_cast<Texture2D::Impl*>(_impl.get())->GetCompressionQuality();
}


void Texture2D::SetCompressed(bool compressed)
{
    return static_cast<Texture2D::Impl*>(_impl.get())->SetCompressed(compressed);
}

bool Texture2D::GetCompressed() const
{
    return static_cast<Texture2D::Impl*>(_impl.get())->GetCompressed();;
}
