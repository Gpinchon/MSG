#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Core/Inherit.hpp>
#include <MSG/Core/Object.hpp>
#include <MSG/Core/Property.hpp>
#include <MSG/Image.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
class Sampler;
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace MSG {
enum class TextureType {
    Unknown = -1,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture2DMultisample,
    Texture2DMultisampleArray,
    Texture3D,
    TextureBuffer,
    TextureCubemap,
    TextureCubemapArray,
    TextureRectangle,
    MaxValue
};

using TextureBase = std::vector<std::shared_ptr<Image>>;
class Texture : public TextureBase, public Core::Inherit<Core::Object, Texture> {
public:
    PROPERTY(TextureType, Type, TextureType::Unknown);
    PROPERTY(PixelDescriptor, PixelDescriptor, );
    PROPERTY(glm::uvec3, Size, {});
    PROPERTY(glm::uvec3, Offset, {});
    PROPERTY(bool, Compressed, false);
    PROPERTY(uint8_t, CompressionQuality, 255);

public:
    using TextureBase::TextureBase;
    using TextureBase::operator=;
    using TextureBase::operator[];
    Texture(const TextureType& a_Type)
        : Inherit()
    {
        SetType(a_Type);
    }
    Texture(const TextureType& a_Type, const std::shared_ptr<Image>& a_Image)
        : Texture(a_Type)
    {
        SetPixelDescriptor(a_Image->GetPixelDescriptor());
        SetSize(a_Image->GetSize());
        emplace_back(a_Image);
    }
    /**
     * @brief automatically generate mipmaps.
     * Base level has to be set.
     * The nbr of mipmaps is computed with : floor(log2(max(size.x, size.y[, size.z])))
     * It is recommended to generate mipmaps BEFORE compressing the texture
     */
    void GenerateMipmaps();
    /**
     * @brief replaces the stored Core::Images by compressed versions
     * @param a_Quality the quality level [0..255]
     */
    void Compress(const uint8_t& a_Quality);
    /**
     * @brief replaces the stored Core::Images by decompressed versions
     */
    void Decompress();
};
}
