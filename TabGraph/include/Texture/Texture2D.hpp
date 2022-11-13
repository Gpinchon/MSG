#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <Texture/Texture.hpp>

#include <glm/vec2.hpp>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Assets {
class Asset;
}

////////////////////////////////////////////////////////////////////////////////
// Defines
////////////////////////////////////////////////////////////////////////////////
#define MIPMAPNBR(size) int((size.x <= 0 && size.y <= 0) ? 0 : floor(log2(std::max(size.x, size.y))))

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Textures {
class Texture2D : public Texture {
public:
    class Impl;
    friend Impl;

public:
    Texture2D(const Texture2D&);
    /**
     * @param size : the resolutionin pixels
     * @param internalFormat : the sized format that will be used to store the texture on the GPU
    */
    Texture2D(const glm::ivec2& size, const Pixel::Description& pixelDesc);
    /**
     * @brief Creates a Texture2D with an image to load from
     * the image is released when loading is done with RemoveComponent
     * @param image the image to use for loading, released when loading is done
    */
    Texture2D(std::shared_ptr<Assets::Asset> image);
    void SetSize(const glm::ivec2& size);
    glm::ivec2 GetSize() const;
    void SetCompressionQuality(float compression);
    float GetCompressionQuality() const;
    void SetCompressed(bool compressed);
    bool GetCompressed() const;
};
}
