/*
* @Author: gpinchon
* @Date:   2021-01-11 08:40:24
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-06-04 12:20:42
*/

#include <Assets/Image.hpp>

#include <glm/glm.hpp>

namespace TabGraph::Assets {
Image::Image(const glm::ivec2& size, Pixel::Description pixelDescription, const std::vector<std::byte>& rawData)
    : Inherit()
    , _data(rawData)
    , _size(size)
{
    static size_t s_imageNbr = 0;
    SetName("Image_" + std::to_string(++s_imageNbr));
    SetPixelDescription(pixelDescription);
    auto rawDataSize { size.x * size.y * GetPixelDescription().GetSize() };
    if (!rawData.empty())
        assert(rawData.size() == rawDataSize);
    _data.resize(rawDataSize);
}

void Image::SetData(const std::vector<std::byte>& data)
{
    _data = data;
}

std::vector<std::byte>& Image::GetData()
{
    return _data;
}

#define CLAMPX(texX) glm::clamp(int(texX), 0, GetSize().x - 1)
#define CLAMPY(texY) glm::clamp(int(texY), 0, GetSize().y - 1)

Pixel::Color Image::GetColor(const glm::vec2& texCoord, Image::SamplingFilter filter)
{
    assert(!GetData().empty() && "Image::GetColor : Unpacked Data is empty");
    if (filter == Image::SamplingFilter::Nearest)
        return GetPixelDescription().GetColorFromBytes(_GetPointer(texCoord));
    else {
        return Pixel::BilinearFilter(glm::fract(texCoord.x), glm::fract(texCoord.y),
            GetColor(glm::ivec2(texCoord.x, texCoord.y)), GetColor(glm::ivec2(CLAMPX(texCoord.x + 1), texCoord.y)),
            GetColor(glm::ivec2(texCoord.x, CLAMPY(texCoord.y + 1))), GetColor(glm::ivec2(CLAMPX(texCoord.x + 1), CLAMPY(texCoord.y + 1))));
    }
}

void Image::SetColor(const glm::ivec2& texCoord, const glm::vec4& color)
{
    assert(!GetData().empty() && "Image::SetColor : Unpacked Data is empty");
    GetPixelDescription().SetColorToBytes(_GetPointer(texCoord), color);
}

void Image::SetPixelDescription(Pixel::Description pixelFormat)
{
    _SetPixelDescription(pixelFormat);
}

void Image::SetSize(const glm::ivec2& size, SamplingFilter filter)
{
    if (size == GetSize())
        return;
    auto newImage = Image(size, GetPixelDescription()); //Create empty image
    for (auto y0 = 0, y1 = size.y / 2; y0 < size.y / 2 && y1 < size.y; ++y0, ++y1) {
        for (auto x0 = 0, x1 = size.x / 2; x0 < size.x / 2 && x1 < size.x; ++x0, ++x1) {
            auto u0 = x0 / float(size.x);
            auto u1 = x1 / float(size.x);
            auto v0 = y0 / float(size.y);
            auto v1 = y1 / float(size.y);
            newImage.SetColor(glm::ivec2(x0, y0), GetColor(glm::vec2(u0 * GetSize().x, v0 * GetSize().y), filter));
            newImage.SetColor(glm::ivec2(x0, y1), GetColor(glm::vec2(u0 * GetSize().x, v1 * GetSize().y), filter));
            newImage.SetColor(glm::ivec2(x1, y0), GetColor(glm::vec2(u1 * GetSize().x, v0 * GetSize().y), filter));
            newImage.SetColor(glm::ivec2(x1, y1), GetColor(glm::vec2(u1 * GetSize().x, v1 * GetSize().y), filter));
        }
    }
    _data = newImage._data;
    _data.shrink_to_fit();
    _size = newImage._size;
}

std::byte* Image::_GetPointer(glm::ivec2 texCoord)
{
    auto index = GetPixelDescription().GetPixelIndex(GetSize(), texCoord);
    assert((index + GetPixelDescription().GetSize()) <= GetData().size() && "Image::_GetPointer : Unpacked Data index out of bound");
    return GetData().data() + index;
}
}