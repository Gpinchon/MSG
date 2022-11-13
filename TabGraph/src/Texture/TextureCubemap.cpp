/*
* @Author: gpinchon
* @Date:   2019-02-22 16:13:28
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:24
*/

#include <Texture/TextureCubemap.hpp>
#include <Assets/Asset.hpp>
#include <Assets/Image.hpp>
#include <Config.hpp>
#include <Debug.hpp>
#include <Engine.hpp>
#include <Tools/Tools.hpp>

#if RENDERINGAPI == OpenGL
#include <Driver/OpenGL/Texture/TextureCubemap.hpp>
#endif

#include <glm/glm.hpp>
#include <iostream>
#include <math.h>
#include <stdexcept>
#include <thread>

namespace TabGraph::Textures {
TextureCubemap::TextureCubemap(const TextureCubemap& other)
    : Inherit(other)
{
    _impl.reset(new TextureCubemap::Impl(static_cast<const TextureCubemap::Impl&>(*other._impl.get())));
}

TextureCubemap::TextureCubemap(glm::ivec2 size, Pixel::SizedFormat format)
    : Inherit()
{
    _impl.reset(new TextureCubemap::Impl(size, format));
}

TextureCubemap::TextureCubemap(std::shared_ptr<Assets::Asset> image)
    : Inherit()
{
    _impl.reset(new TextureCubemap::Impl(image));
}

TextureCubemap::~TextureCubemap()
{
}

glm::vec3 outImgToXYZ(float u, float v, int faceIdx)
{
    glm::vec3 xyz;
    auto a = 2.0 * u;
    auto b = 2.0 * v;

    if (faceIdx == 0) // back
        xyz = glm::vec3(-1.0, 1.0 - a, 1.0 - b);
    else if (faceIdx == 1) // left
        xyz = glm::vec3(a - 1.0, -1.0, 1.0 - b);
    else if (faceIdx == 2) // front)
        xyz = glm::vec3(1.0, a - 1.0, 1.0 - b);
    else if (faceIdx == 3) // right)
        xyz = glm::vec3(1.0 - a, 1.0, 1.0 - b);
    else if (faceIdx == 4) // top
        xyz = glm::vec3(b - 1.0, a - 1.0, 1.0);
    else if (faceIdx == 5) // bottom
        xyz = glm::vec3(1.0 - b, a - 1.0, -1.0);
    return (xyz);
}

auto HDRColor(std::shared_ptr<Assets::Image> hdr_image,
    float dx, float dy, float dz)
{
    // assume angle map projection
    const float one_over_pi = 1.0f / M_PI;
    float invl = 1.0f / sqrtf(dx * dx + dy * dy);
    float r = one_over_pi * acosf(dz) * invl;
    float u = dx * r; // -1..1
    float v = dy * r; // -1..1
    // map to pixel coordinates
    int x = int(u * hdr_image->GetSize().x + hdr_image->GetSize().x) >> 1;
    int y = int(v * hdr_image->GetSize().y + hdr_image->GetSize().y) >> 1;
    // return the float RGB value at (x,y)
    return hdr_image->GetColor(glm::ivec2(x, y));
}

glm::vec3 CubeMapUVToXYZ(const int index, glm::vec2 uv)
{
    glm::vec3 xyz = glm::vec3(0);
    // convert range 0 to 1 to -1 to 1
    uv = uv * 2.f - 1.f;
    switch (index) {
    case 0:
        xyz = glm::vec3(1.0f, -uv.y, -uv.x);
        break; // POSITIVE X
    case 1:
        xyz = glm::vec3(-1.0f, -uv.y, uv.x);
        break; // NEGATIVE X
    case 2:
        xyz = glm::vec3(uv.x, 1.0f, uv.y);
        break; // POSITIVE Y
    case 3:
        xyz = glm::vec3(uv.x, -1.0f, -uv.y);
        break; // NEGATIVE Y
    case 4:
        xyz = glm::vec3(uv.x, -uv.y, 1.0f);
        break; // POSITIVE Z
    case 5:
        xyz = glm::vec3(-uv.x, -uv.y, -1.0f);
        break; // NEGATIVE Z
    }
    return normalize(xyz);
}

const glm::vec2 invAtan = glm::vec2(0.1591, 0.3183);
glm::vec2 XYZToEquirectangular(glm::vec3 xyz)
{
    glm::vec2 uv = glm::vec2(atan2(xyz.z, xyz.x), asin(xyz.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void TextureCubemap::ExtractSide(std::shared_ptr<Assets::Image> fromImage, std::shared_ptr<Assets::Image> toImage, Side side)
{
    for (auto x = 0; x < toImage->GetSize().x; ++x) {
        for (auto y = 0; y < toImage->GetSize().y; ++y) {
            float nx = std::clamp((float)x / ((float)toImage->GetSize().x - 0.5f), 0.f, 1.f);
            float ny = std::clamp((float)y / ((float)toImage->GetSize().y - 0.5f), 0.f, 1.f);
            auto xyz = CubeMapUVToXYZ((int)side, glm::vec2(nx, ny));
            auto uv = XYZToEquirectangular(xyz);
            glm::vec2 sampleTexCoord { uv * glm::vec2(fromImage->GetSize()) };
            sampleTexCoord = glm::clamp(sampleTexCoord, glm::vec2(0), glm::vec2(fromImage->GetSize() - 1));
            auto color { fromImage->GetColor(sampleTexCoord, Assets::Image::SamplingFilter::Bilinear) };
            toImage->SetColor(glm::vec2(x, y), color);
        }
    }
}

glm::ivec2 TextureCubemap::GetSize() const
{
    return static_cast<TextureCubemap::Impl*>(_impl.get())->GetSize();
}

void TextureCubemap::SetSize(glm::ivec2 size)
{
    return static_cast<TextureCubemap::Impl*>(_impl.get())->SetSize(size);
}

}
