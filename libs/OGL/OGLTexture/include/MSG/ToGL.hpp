#pragma once

#pragma once

#include <MSG/PixelDescriptor.hpp>

#include <stdexcept>

#include <GL/glew.h>

namespace Msg {
static inline auto ToGL(const Core::DataType& a_Type)
{
    switch (a_Type) {
    case Core::DataType::Uint8:
        return GL_UNSIGNED_BYTE;
    case Core::DataType::Int8:
        return GL_BYTE;
    case Core::DataType::Uint16:
        return GL_UNSIGNED_SHORT;
    case Core::DataType::Int16:
        return GL_SHORT;
    case Core::DataType::Uint32:
        return GL_UNSIGNED_INT;
    case Core::DataType::Int32:
        return GL_INT;
    case Core::DataType::Float16:
        return GL_HALF_FLOAT;
    case Core::DataType::Float32:
        return GL_FLOAT;
    default:
        throw std::runtime_error("Unknown Pixel Type");
    }
    return GL_NONE;
}

static inline auto ToGL(const PixelUnsizedFormat& a_UnsizedFormat)
{
    switch (a_UnsizedFormat) {
    case PixelUnsizedFormat::R:
    case PixelUnsizedFormat::R_Integer:
        return GL_RED;
    case PixelUnsizedFormat::RG:
    case PixelUnsizedFormat::RG_Integer:
        return GL_RG;
    case PixelUnsizedFormat::RGB:
    case PixelUnsizedFormat::RGB_Integer:
        return GL_RGB;
    case PixelUnsizedFormat::RGBA:
    case PixelUnsizedFormat::RGBA_Integer:
        return GL_RGBA;
    case PixelUnsizedFormat::Depth:
        return GL_DEPTH_COMPONENT;
    case PixelUnsizedFormat::Depth_Stencil:
        return GL_DEPTH_STENCIL;
    case PixelUnsizedFormat::Stencil:
        return GL_STENCIL_INDEX;
    default:
        throw std::runtime_error("Unknown Pixel Unsized Format");
    }
    return GL_NONE;
}

static inline auto ToGL(const PixelSizedFormat& a_SizedFormat)
{
    switch (a_SizedFormat) {
    case PixelSizedFormat::Uint8_NormalizedR:
        return GL_R8;
    case PixelSizedFormat::Uint8_NormalizedRG:
        return GL_RG8;
    case PixelSizedFormat::Uint8_NormalizedRGB:
        return GL_RGB8;
    case PixelSizedFormat::Uint8_NormalizedRGBA:
        return GL_RGBA8;
    case PixelSizedFormat::Int8_NormalizedR:
        return GL_RGB8_SNORM;
    case PixelSizedFormat::Int8_NormalizedRG:
        return GL_RG8_SNORM;
    case PixelSizedFormat::Int8_NormalizedRGB:
        return GL_RGB8_SNORM;
    case PixelSizedFormat::Int8_NormalizedRGBA:
        return GL_RGBA8_SNORM;
    case PixelSizedFormat::Uint8_R:
        return GL_R8UI;
    case PixelSizedFormat::Uint8_RG:
        return GL_RG8UI;
    case PixelSizedFormat::Uint8_RGB:
        return GL_RGB8UI;
    case PixelSizedFormat::Uint8_RGBA:
        return GL_RGBA8UI;
    case PixelSizedFormat::Int8_R:
        return GL_R8I;
    case PixelSizedFormat::Int8_RG:
        return GL_RG8I;
    case PixelSizedFormat::Int8_RGB:
        return GL_RGB8I;
    case PixelSizedFormat::Int8_RGBA:
        return GL_RGBA8I;
    case PixelSizedFormat::Uint16_NormalizedR:
        return GL_R16;
    case PixelSizedFormat::Uint16_NormalizedRG:
        return GL_RG16;
    case PixelSizedFormat::Uint16_NormalizedRGB:
        return GL_RGB16;
    case PixelSizedFormat::Uint16_NormalizedRGBA:
        return GL_RGBA16;
    case PixelSizedFormat::Int16_NormalizedR:
        return GL_R16_SNORM;
    case PixelSizedFormat::Int16_NormalizedRG:
        return GL_RG16_SNORM;
    case PixelSizedFormat::Int16_NormalizedRGB:
        return GL_RGB16_SNORM;
    case PixelSizedFormat::Int16_NormalizedRGBA:
        return GL_RGBA16_SNORM;
    case PixelSizedFormat::Uint16_R:
        return GL_R16UI;
    case PixelSizedFormat::Uint16_RG:
        return GL_RG16UI;
    case PixelSizedFormat::Uint16_RGB:
        return GL_RGB16UI;
    case PixelSizedFormat::Uint16_RGBA:
        return GL_RGBA16UI;
    case PixelSizedFormat::Int16_R:
        return GL_R16I;
    case PixelSizedFormat::Int16_RG:
        return GL_RG16I;
    case PixelSizedFormat::Int16_RGB:
        return GL_RGB16I;
    case PixelSizedFormat::Int16_RGBA:
        return GL_RGBA16I;
    case PixelSizedFormat::Uint32_R:
        return GL_R32UI;
    case PixelSizedFormat::Uint32_RG:
        return GL_RG32UI;
    case PixelSizedFormat::Uint32_RGB:
        return GL_RGB32UI;
    case PixelSizedFormat::Uint32_RGBA:
        return GL_RGBA32UI;
    case PixelSizedFormat::Int32_R:
        return GL_R32I;
    case PixelSizedFormat::Int32_RG:
        return GL_RG32I;
    case PixelSizedFormat::Int32_RGB:
        return GL_RGB32I;
    case PixelSizedFormat::Int32_RGBA:
        return GL_RGBA32I;
    case PixelSizedFormat::Float16_R:
        return GL_R16F;
    case PixelSizedFormat::Float16_RG:
        return GL_RG16F;
    case PixelSizedFormat::Float16_RGB:
        return GL_RGB16F;
    case PixelSizedFormat::Float16_RGBA:
        return GL_RGBA16F;
    case PixelSizedFormat::Float32_R:
        return GL_R32F;
    case PixelSizedFormat::Float32_RG:
        return GL_RG32F;
    case PixelSizedFormat::Float32_RGB:
        return GL_RGB32F;
    case PixelSizedFormat::Float32_RGBA:
        return GL_RGBA32F;
    case PixelSizedFormat::Depth16:
        return GL_DEPTH_COMPONENT16;
    case PixelSizedFormat::Depth24:
        return GL_DEPTH_COMPONENT24;
    case PixelSizedFormat::Depth32:
        return GL_DEPTH_COMPONENT32;
    case PixelSizedFormat::Depth32F:
        return GL_DEPTH_COMPONENT32F;
    case PixelSizedFormat::Depth24_Stencil8:
        return GL_DEPTH24_STENCIL8;
    case PixelSizedFormat::Depth32F_Stencil8:
        return GL_DEPTH32F_STENCIL8;
    case PixelSizedFormat::Stencil8:
        return GL_STENCIL_INDEX8;
    case PixelSizedFormat::DXT5_RGBA:
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    default:
        throw std::runtime_error("Unknown Pixel Sized Format");
    }
    return GL_NONE;
}
}
