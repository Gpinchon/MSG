/*
 * @Author: gpinchon
 * @Date:   2019-02-22 16:19:03
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-04 20:09:23
 */

#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <SG/Core/Image/Pixel.hpp>
#include <SG/Core/Inherit.hpp>
#include <SG/Core/Object.hpp>
#include <SG/Core/Property.hpp>

#include <map>
#include <memory>

////////////////////////////////////////////////////////////////////////////////
// Forward declarations
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Sampler;
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
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
class Texture : public Inherit<Object, Texture> {
public:
    PROPERTY(TextureType, Type, TextureType::Unknown);
    PROPERTY(Pixel::Description, PixelDescription, {});
    PROPERTY(std::vector<std::shared_ptr<Image>>, Levels, );
    PROPERTY(glm::uvec3, Size, {});
    PROPERTY(glm::uvec3, Offset, {});
    PROPERTY(bool, Compressed, false);
    PROPERTY(float, CompressionQuality, 1);

public:
    Texture(const TextureType& a_Type)
        : Inherit()
    {
        SetType(a_Type);
    }
    Texture(const TextureType& a_Type, const std::shared_ptr<SG::Image>& a_Image)
        : Texture(a_Type)
    {
        SetPixelDescription(a_Image->GetPixelDescription());
        SetSize(a_Image->GetSize());
        SetLevels({ a_Image });
    }
    auto& operator[](size_t a_Index)
    {
        return GetLevels().at(a_Index);
    }
    auto& operator[](size_t a_Index) const
    {
        return GetLevels().at(a_Index);
    }
    /// @brief automatically generate mipmaps.
    /// Base level has to be set.
    /// The nbr of mipmaps is computed with : floor(log2(max(size.x, size.y[, size.z])))
    void GenerateMipmaps();
};
}
