#pragma once

#include <Core/Image/Pixel.hpp>
#include <Core/Primitive.hpp>
#include <Core/Texture/Sampler.hpp>
#include <Renderer/OGL/RAII/Sampler.hpp>

#include <stdexcept>

#include <GL/glew.h>

namespace MSG::Renderer {
static inline auto ToGL(const Core::Primitive::DrawingMode& a_DrawMode)
{
    switch (a_DrawMode) {
    case Core::Primitive::DrawingMode::Points:
        return GL_POINTS;
    case Core::Primitive::DrawingMode::Lines:
        return GL_LINES;
    case Core::Primitive::DrawingMode::LineStrip:
        return GL_LINE_STRIP;
    case Core::Primitive::DrawingMode::LineLoop:
        return GL_LINE_LOOP;
    case Core::Primitive::DrawingMode::Polygon:
        return GL_POLYGON;
    case Core::Primitive::DrawingMode::Triangles:
        return GL_TRIANGLES;
    case Core::Primitive::DrawingMode::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    case Core::Primitive::DrawingMode::TriangleFan:
        return GL_TRIANGLE_FAN;
    case Core::Primitive::DrawingMode::Quads:
        return GL_QUADS;
    case Core::Primitive::DrawingMode::QuadStrip:
        return GL_QUAD_STRIP;
    default:
        throw std::runtime_error("Unknown Draw Mode");
    }
    return GL_NONE;
}

static inline auto ToGL(const Core::Sampler::CompareFunc& a_Func)
{
    switch (a_Func) {
    case Core::Sampler::CompareFunc::LessEqual:
        return GL_LEQUAL;
    case Core::Sampler::CompareFunc::GreaterEqual:
        return GL_GEQUAL;
    case Core::Sampler::CompareFunc::Less:
        return GL_LESS;
    case Core::Sampler::CompareFunc::Greater:
        return GL_GREATER;
    case Core::Sampler::CompareFunc::Equal:
        return GL_EQUAL;
    case Core::Sampler::CompareFunc::NotEqual:
        return GL_NOTEQUAL;
    case Core::Sampler::CompareFunc::Always:
        return GL_ALWAYS;
    case Core::Sampler::CompareFunc::Never:
        return GL_NEVER;
    default:
        throw std::runtime_error("Unknown Compare Func");
    }
    return GL_NONE;
}

static inline auto ToGL(const Core::Sampler::CompareMode& a_Mode)
{
    switch (a_Mode) {
    case Core::Sampler::CompareMode::None:
        return GL_NONE;
    case Core::Sampler::CompareMode::CompareRefToTexture:
        return GL_COMPARE_REF_TO_TEXTURE;
    default:
        throw std::runtime_error("Unknown Compare Mode");
    }
    return GL_NONE;
}

static inline auto ToGL(const Core::Sampler::Filter& a_Filter)
{
    switch (a_Filter) {
    case Core::Sampler::Filter::Nearest:
        return GL_NEAREST;
    case Core::Sampler::Filter::Linear:
        return GL_LINEAR;
    case Core::Sampler::Filter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case Core::Sampler::Filter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case Core::Sampler::Filter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case Core::Sampler::Filter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    default:
        throw std::runtime_error("Unknown Filter");
    }
    return GL_NONE;
}

static inline auto ToGL(const Core::Sampler::Wrap& a_Wrap)
{
    switch (a_Wrap) {
    case Core::Sampler::Wrap::Repeat:
        return GL_REPEAT;
    case Core::Sampler::Wrap::ClampToBorder:
        return GL_CLAMP_TO_BORDER;
    case Core::Sampler::Wrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case Core::Sampler::Wrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    case Core::Sampler::Wrap::MirroredClampToEdge:
        return GL_MIRROR_CLAMP_TO_EDGE;
    default:
        throw std::runtime_error("Unknown Wrap");
    }
    return GL_NONE;
}

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

static inline auto ToGL(const Core::Pixel::UnsizedFormat& a_UnsizedFormat)
{
    switch (a_UnsizedFormat) {
    case Core::Pixel::UnsizedFormat::R:
    case Core::Pixel::UnsizedFormat::R_Integer:
        return GL_R;
    case Core::Pixel::UnsizedFormat::RG:
    case Core::Pixel::UnsizedFormat::RG_Integer:
        return GL_RG;
    case Core::Pixel::UnsizedFormat::RGB:
    case Core::Pixel::UnsizedFormat::RGB_Integer:
        return GL_RGB;
    case Core::Pixel::UnsizedFormat::RGBA:
    case Core::Pixel::UnsizedFormat::RGBA_Integer:
        return GL_RGBA;
    case Core::Pixel::UnsizedFormat::Depth:
        return GL_DEPTH_COMPONENT;
    case Core::Pixel::UnsizedFormat::Depth_Stencil:
        return GL_DEPTH_STENCIL;
    case Core::Pixel::UnsizedFormat::Stencil:
        return GL_STENCIL_INDEX;
    default:
        throw std::runtime_error("Unknown Pixel Unsized Format");
    }
    return GL_NONE;
}

static inline auto ToGL(const Core::Pixel::SizedFormat& a_SizedFormat)
{
    switch (a_SizedFormat) {
    case Core::Pixel::SizedFormat::Uint8_NormalizedR:
        return GL_R8;
    case Core::Pixel::SizedFormat::Uint8_NormalizedRG:
        return GL_RG8;
    case Core::Pixel::SizedFormat::Uint8_NormalizedRGB:
        return GL_RGB8;
    case Core::Pixel::SizedFormat::Uint8_NormalizedRGBA:
        return GL_RGBA8;
    case Core::Pixel::SizedFormat::Int8_NormalizedR:
        return GL_RGB8_SNORM;
    case Core::Pixel::SizedFormat::Int8_NormalizedRG:
        return GL_RG8_SNORM;
    case Core::Pixel::SizedFormat::Int8_NormalizedRGB:
        return GL_RGB8_SNORM;
    case Core::Pixel::SizedFormat::Int8_NormalizedRGBA:
        return GL_RGBA8_SNORM;
    case Core::Pixel::SizedFormat::Uint8_R:
        return GL_R8UI;
    case Core::Pixel::SizedFormat::Uint8_RG:
        return GL_RG8UI;
    case Core::Pixel::SizedFormat::Uint8_RGB:
        return GL_RGB8UI;
    case Core::Pixel::SizedFormat::Uint8_RGBA:
        return GL_RGBA8UI;
    case Core::Pixel::SizedFormat::Int8_R:
        return GL_R8I;
    case Core::Pixel::SizedFormat::Int8_RG:
        return GL_RG8I;
    case Core::Pixel::SizedFormat::Int8_RGB:
        return GL_RGB8I;
    case Core::Pixel::SizedFormat::Int8_RGBA:
        return GL_RGBA8I;
    case Core::Pixel::SizedFormat::Uint16_NormalizedR:
        return GL_R16;
    case Core::Pixel::SizedFormat::Uint16_NormalizedRG:
        return GL_RG16;
    case Core::Pixel::SizedFormat::Uint16_NormalizedRGB:
        return GL_RGB16;
    case Core::Pixel::SizedFormat::Uint16_NormalizedRGBA:
        return GL_RGBA16;
    case Core::Pixel::SizedFormat::Int16_NormalizedR:
        return GL_R16_SNORM;
    case Core::Pixel::SizedFormat::Int16_NormalizedRG:
        return GL_RG16_SNORM;
    case Core::Pixel::SizedFormat::Int16_NormalizedRGB:
        return GL_RGB16_SNORM;
    case Core::Pixel::SizedFormat::Int16_NormalizedRGBA:
        return GL_RGBA16_SNORM;
    case Core::Pixel::SizedFormat::Uint16_R:
        return GL_R16UI;
    case Core::Pixel::SizedFormat::Uint16_RG:
        return GL_RG16UI;
    case Core::Pixel::SizedFormat::Uint16_RGB:
        return GL_RGB16UI;
    case Core::Pixel::SizedFormat::Uint16_RGBA:
        return GL_RGBA16UI;
    case Core::Pixel::SizedFormat::Int16_R:
        return GL_R16I;
    case Core::Pixel::SizedFormat::Int16_RG:
        return GL_RG16I;
    case Core::Pixel::SizedFormat::Int16_RGB:
        return GL_RGB16I;
    case Core::Pixel::SizedFormat::Int16_RGBA:
        return GL_RGBA16I;
    case Core::Pixel::SizedFormat::Uint32_R:
        return GL_R32UI;
    case Core::Pixel::SizedFormat::Uint32_RG:
        return GL_RG32UI;
    case Core::Pixel::SizedFormat::Uint32_RGB:
        return GL_RGB32UI;
    case Core::Pixel::SizedFormat::Uint32_RGBA:
        return GL_RGBA32UI;
    case Core::Pixel::SizedFormat::Int32_R:
        return GL_R32I;
    case Core::Pixel::SizedFormat::Int32_RG:
        return GL_RG32I;
    case Core::Pixel::SizedFormat::Int32_RGB:
        return GL_RGB32I;
    case Core::Pixel::SizedFormat::Int32_RGBA:
        return GL_RGBA32I;
    case Core::Pixel::SizedFormat::Float16_R:
        return GL_R16F;
    case Core::Pixel::SizedFormat::Float16_RG:
        return GL_RG16F;
    case Core::Pixel::SizedFormat::Float16_RGB:
        return GL_RGB16F;
    case Core::Pixel::SizedFormat::Float16_RGBA:
        return GL_RGBA16F;
    case Core::Pixel::SizedFormat::Float32_R:
        return GL_R32F;
    case Core::Pixel::SizedFormat::Float32_RG:
        return GL_RG32F;
    case Core::Pixel::SizedFormat::Float32_RGB:
        return GL_RGB32F;
    case Core::Pixel::SizedFormat::Float32_RGBA:
        return GL_RGBA32F;
    case Core::Pixel::SizedFormat::Depth16:
        return GL_DEPTH_COMPONENT16;
    case Core::Pixel::SizedFormat::Depth24:
        return GL_DEPTH_COMPONENT24;
    case Core::Pixel::SizedFormat::Depth32:
        return GL_DEPTH_COMPONENT32;
    case Core::Pixel::SizedFormat::Depth32F:
        return GL_DEPTH_COMPONENT32F;
    case Core::Pixel::SizedFormat::Depth24_Stencil8:
        return GL_DEPTH24_STENCIL8;
    case Core::Pixel::SizedFormat::Depth32F_Stencil8:
        return GL_DEPTH32F_STENCIL8;
    case Core::Pixel::SizedFormat::Stencil8:
        return GL_STENCIL_INDEX8;
    case Core::Pixel::SizedFormat::DXT5_RGBA:
        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    default:
        throw std::runtime_error("Unknown Pixel Sized Format");
    }
    return GL_NONE;
}

static inline auto ToGL(const Core::Sampler& a_Sampler)
{
    RAII::SamplerParameters parameters {};
    parameters.borderColor = a_Sampler.GetBorderColor();
    parameters.compareFunc = ToGL(a_Sampler.GetCompareFunc());
    parameters.compareMode = ToGL(a_Sampler.GetCompareMode());
    parameters.magFilter   = ToGL(a_Sampler.GetMagFilter());
    parameters.maxLOD      = a_Sampler.GetMaxLOD();
    parameters.minFilter   = ToGL(a_Sampler.GetMinFilter());
    parameters.minLOD      = a_Sampler.GetMinLOD();
    parameters.wrapR       = ToGL(a_Sampler.GetWrapR());
    parameters.wrapS       = ToGL(a_Sampler.GetWrapS());
    parameters.wrapT       = ToGL(a_Sampler.GetWrapT());
    return parameters;
}
}
