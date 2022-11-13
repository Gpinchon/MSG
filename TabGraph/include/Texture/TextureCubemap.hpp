/*
* @Author: gpinchon
* @Date:   2019-02-22 16:19:03
* @Last Modified by:   gpinchon
* @Last Modified time: 2021-05-04 20:02:25
*/

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Texture/Texture.hpp>

#include <glm/vec2.hpp>
#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class Asset;
class Image;
}


#define MIPMAPNBR(size) int((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(std::max(size.x, size.y))))

////////////////////////////////////////////////////////////////////////////////
// Class declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class TextureCubemap : public Core::Inherit<Texture, TextureCubemap> {
public:
    class Impl;
    friend Impl;
    enum class Side {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ
    };
    TextureCubemap() = delete;
    TextureCubemap(const TextureCubemap&);
    TextureCubemap(glm::ivec2 size, Pixel::SizedFormat format);
    TextureCubemap(std::shared_ptr<Assets::Asset> image);
    ~TextureCubemap();
    /**
     * @brief extracts TextureCubemap's side from equirectangular image
     * @param fromImage the equirectangular image to be used
     * @param toImage the face to extract
     *
     */
    static void ExtractSide(std::shared_ptr<Assets::Image> fromImage, std::shared_ptr<Assets::Image> toImage, TextureCubemap::Side side);
    glm::ivec2 GetSize() const;
    void SetSize(glm::ivec2 size);
};
}
