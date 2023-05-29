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
class TextureSampler;
class Image;
}

////////////////////////////////////////////////////////////////////////////////
// Class declaration
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::SG {
class Texture : public Inherit<Object, Texture> {
public:
    class Impl;
    enum class Type {
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
    PROPERTY(Type, Type, Type::Unknown);
    PROPERTY(Pixel::Description, PixelDescription, {});
    PROPERTY(std::shared_ptr<TextureSampler>, Sampler, nullptr);
    PROPERTY(std::shared_ptr<Image>, Image, nullptr);

public:
    Texture(const Type& a_Type)
        : Inherit()
    {
        SetType(a_Type);
    }
};
}
