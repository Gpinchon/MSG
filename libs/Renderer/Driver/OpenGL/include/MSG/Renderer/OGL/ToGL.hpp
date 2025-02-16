#pragma once

#include <MSG/Mesh/Primitive.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/PixelDescriptor.hpp>
#include <MSG/Sampler.hpp>

#include <stdexcept>

#include <GL/glew.h>

namespace MSG::Renderer {
static inline auto ToGL(const MeshPrimitive::DrawingMode& a_DrawMode)
{
    switch (a_DrawMode) {
    case MeshPrimitive::DrawingMode::Points:
        return GL_POINTS;
    case MeshPrimitive::DrawingMode::Lines:
        return GL_LINES;
    case MeshPrimitive::DrawingMode::LineStrip:
        return GL_LINE_STRIP;
    case MeshPrimitive::DrawingMode::LineLoop:
        return GL_LINE_LOOP;
    case MeshPrimitive::DrawingMode::Polygon:
        return GL_POLYGON;
    case MeshPrimitive::DrawingMode::Triangles:
        return GL_TRIANGLES;
    case MeshPrimitive::DrawingMode::TriangleStrip:
        return GL_TRIANGLE_STRIP;
    case MeshPrimitive::DrawingMode::TriangleFan:
        return GL_TRIANGLE_FAN;
    case MeshPrimitive::DrawingMode::Quads:
        return GL_QUADS;
    case MeshPrimitive::DrawingMode::QuadStrip:
        return GL_QUAD_STRIP;
    default:
        throw std::runtime_error("Unknown Draw Mode");
    }
    return GL_NONE;
}

static inline auto ToGL(const SamplerCompareFunc& a_Func)
{
    switch (a_Func) {
    case SamplerCompareFunc::LessEqual:
        return GL_LEQUAL;
    case SamplerCompareFunc::GreaterEqual:
        return GL_GEQUAL;
    case SamplerCompareFunc::Less:
        return GL_LESS;
    case SamplerCompareFunc::Greater:
        return GL_GREATER;
    case SamplerCompareFunc::Equal:
        return GL_EQUAL;
    case SamplerCompareFunc::NotEqual:
        return GL_NOTEQUAL;
    case SamplerCompareFunc::Always:
        return GL_ALWAYS;
    case SamplerCompareFunc::Never:
        return GL_NEVER;
    default:
        throw std::runtime_error("Unknown Compare Func");
    }
    return GL_NONE;
}

static inline auto ToGL(const SamplerCompareMode& a_Mode)
{
    switch (a_Mode) {
    case SamplerCompareMode::None:
        return GL_NONE;
    case SamplerCompareMode::CompareRefToTexture:
        return GL_COMPARE_REF_TO_TEXTURE;
    default:
        throw std::runtime_error("Unknown Compare Mode");
    }
    return GL_NONE;
}

static inline auto ToGL(const SamplerFilter& a_Filter)
{
    switch (a_Filter) {
    case SamplerFilter::Nearest:
        return GL_NEAREST;
    case SamplerFilter::Linear:
        return GL_LINEAR;
    case SamplerFilter::NearestMipmapLinear:
        return GL_NEAREST_MIPMAP_LINEAR;
    case SamplerFilter::NearestMipmapNearest:
        return GL_NEAREST_MIPMAP_NEAREST;
    case SamplerFilter::LinearMipmapLinear:
        return GL_LINEAR_MIPMAP_LINEAR;
    case SamplerFilter::LinearMipmapNearest:
        return GL_LINEAR_MIPMAP_NEAREST;
    default:
        throw std::runtime_error("Unknown Filter");
    }
    return GL_NONE;
}

static inline auto ToGL(const SamplerWrap& a_Wrap)
{
    switch (a_Wrap) {
    case SamplerWrap::Repeat:
        return GL_REPEAT;
    case SamplerWrap::ClampToBorder:
        return GL_CLAMP_TO_BORDER;
    case SamplerWrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    case SamplerWrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    case SamplerWrap::MirroredClampToEdge:
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

static inline auto ToGL(const PixelUnsizedFormat& a_UnsizedFormat)
{
    switch (a_UnsizedFormat) {
    case PixelUnsizedFormat::R:
    case PixelUnsizedFormat::R_Integer:
        return GL_R;
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

static inline auto ToGL(const Sampler& a_Sampler)
{
    OGLSamplerParameters parameters {};
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
