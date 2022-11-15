/*
* @Author: gpinchon
* @Date:   2021-01-07 17:35:23
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-07-01 22:30:42
*/
#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>
#include <SG/Image/Pixel.hpp>

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
/**
 * @brief Describes an image file located at the path used in the constructor
*/
class Image : public SG::Inherit<SG::Object, Image> {
public:
    enum class SamplingFilter {
        Nearest,
        Bilinear,
        MaxValue
    };
    READONLYPROPERTY(Pixel::Description, PixelDescription);

public:
    static constexpr auto AssetType = "Image";
    /**
     * @brief Constructs a new Image instance
     * @param size The resolution in pixels
     * @param pixelDescription See PixelUtils, can be created using SizedFormat
     * @param rawData The pixel data to be used for the image
    */
    Image(const glm::ivec2& size, Pixel::Description pixelDescription, const std::vector<std::byte>& rawData = {});
    /**
     * @brief 
     * @param data the new unpacked data, data size must be : Size.x * Size.y * Octets Per Pixel
    */
    void SetData(const std::vector<std::byte>& data);
    /**
     * @brief Returns a reference to the Image's raw data
     * @return the image's raw data, data is formated to the PixelFormat
    */
    std::vector<std::byte>& GetData();
    /**
     * @brief Fetches a color from the coordinates, asserts thad _data is not empty
     * @param texCoord the texture coordinate to fetch the color from
     * @param filter the filtering to be used for sampling, default is nearest
     * @return the unpacked color
    */
    Pixel::Color GetColor(const glm::vec2& texCoord, SamplingFilter filter = SamplingFilter::Nearest);
    /**
     * @brief Sets the pixel corresponding to tesCoord to the specified color
     * @param texCoord the texture coordinates to be set
     * @param color : the new color of this pixel
    */
    void SetColor(const glm::ivec2& texCoord, const glm::vec4& color);
    /**
     * @brief Sets the pixel description of this image
     * @param pixelDescription : the new pixel format of this image
    */
    void SetPixelDescription(Pixel::Description pixelDescription);
    /**
     * @brief Sets the size in pixels of this image
     * @param size : the new size of this image
     * @param filter : the filter to be used for resizing
    */
    void SetSize(const glm::ivec2& size, SamplingFilter filter = SamplingFilter::Nearest);
    /** @return the image's size */
    glm::ivec2 GetSize() const
    {
        return _size;
    }

private:
    std::byte* _GetPointer(glm::ivec2 texCoord);
    std::vector<std::byte> _data;
    glm::ivec2 _size { 0 };
};
}