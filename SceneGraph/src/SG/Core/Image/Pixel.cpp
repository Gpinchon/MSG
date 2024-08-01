/*
 * @Author: gpinchon
 * @Date:   2021-01-12 18:26:34
 * @Last Modified by:   gpinchon
 * @Last Modified time: 2021-05-29 16:56:46
 */
#include <SG/Core/Image/Pixel.hpp>

#include <glm/glm.hpp>
#include <stdexcept>

namespace TabGraph::SG::Pixel {
inline SizedFormat GetRSizedformat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedR : SizedFormat::Uint8_R;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedR : SizedFormat::Int8_R;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedR : SizedFormat::Uint16_R;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedR : SizedFormat::Int16_R;
    case Type::Uint32:
        assert(!normalizedType && "uint2 pixel type cannot be normalized");
        return SizedFormat::Uint32_R;
    case Type::Int32:
        assert(!normalizedType && "int32 pixel type cannot be normalized");
        return SizedFormat::Int32_R;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_R;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_R;
    default:
        throw std::runtime_error("Incorrect R format");
    }
}

inline SizedFormat GetRGSizedformat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedRG : SizedFormat::Uint8_RG;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedRG : SizedFormat::Int8_RG;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedRG : SizedFormat::Uint16_RG;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedRG : SizedFormat::Int16_RG;
    case Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return SizedFormat::Uint32_RG;
    case Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return SizedFormat::Int32_RG;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_RG;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_RG;
    default:
        throw std::runtime_error("Incorrect RG format");
    }
}

inline SizedFormat GetRGBSizedformat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedRGB : SizedFormat::Uint8_RGB;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedRGB : SizedFormat::Int8_RGB;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedRGB : SizedFormat::Uint16_RGB;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedRGB : SizedFormat::Int16_RGB;
    case Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return SizedFormat::Uint32_RGB;
    case Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return SizedFormat::Int32_RGB;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_RGB;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_RGB;
    default:
        throw std::runtime_error("Incorrect RGB format");
    }
}

inline SizedFormat GetRGBASizedFormat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return normalizedType ? SizedFormat::Uint8_NormalizedRGBA : SizedFormat::Uint8_RGBA;
    case Type::Int8:
        return normalizedType ? SizedFormat::Int8_NormalizedRGBA : SizedFormat::Int8_RGBA;
    case Type::Uint16:
        return normalizedType ? SizedFormat::Uint16_NormalizedRGBA : SizedFormat::Uint16_RGBA;
    case Type::Int16:
        return normalizedType ? SizedFormat::Int16_NormalizedRGBA : SizedFormat::Int16_RGBA;
    case Type::Uint32:
        assert(!normalizedType && "unsigned int pixel type cannot be normalized");
        return SizedFormat::Uint32_RGBA;
    case Type::Int32:
        assert(!normalizedType && "int pixel type cannot be normalized");
        return SizedFormat::Int32_RGBA;
    case Type::Float16:
        assert(!normalizedType && "float16 pixel type cannot be normalized");
        return SizedFormat::Float16_RGBA;
    case Type::Float32:
        assert(!normalizedType && "float32 pixel type cannot be normalized");
        return SizedFormat::Float32_RGBA;
    case Type::DXT5Block:
        assert(normalizedType && "DXT5 pixel type must be normalized");
        return SizedFormat::DXT5_RGBA;
    default:
        throw std::runtime_error("Incorrect RGBA format");
    }
}

SizedFormat GetDepthSizedformat(Type type, bool normalizedType)
{
    assert(normalizedType);
    switch (type) {
    case Type::Uint32:
        return SizedFormat::Depth32;
    case Type::Float16:
        return SizedFormat::Depth16;
    case Type::Float32:
        return SizedFormat::Depth32F;
    default:
        throw std::runtime_error("Incorrect Depth format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetDepthStencilSizedFormat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint32:
        return SizedFormat::Depth24_Stencil8;
        break;
    case Type::Float32:
        return SizedFormat::Depth32F_Stencil8;
        break;
    default:
        throw std::runtime_error("Incorrect DepthStencil format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetStencilSizedFormat(Type type, bool normalizedType)
{
    switch (type) {
    case Type::Uint8:
        return SizedFormat::Stencil8;
        break;
    case Type::Int8:
        throw std::runtime_error("Stencil texture cannot be of type Int8");
        break;
    case Type::Uint16:
        throw std::runtime_error("Stencil texture cannot be of type Uint16");
        break;
    case Type::Int16:
        throw std::runtime_error("Stencil texture cannot be of type Int16");
        break;
    case Type::Uint32:
        throw std::runtime_error("Stencil texture cannot be of type Uint32");
        break;
    case Type::Int32:
        throw std::runtime_error("Stencil texture cannot be of type Int32");
        break;
    case Type::Float16:
        throw std::runtime_error("Stencil texture cannot be of type Float16");
        break;
    case Type::Float32:
        throw std::runtime_error("Stencil texture cannot be of type Float32");
        break;
    default:
        throw std::runtime_error("Unknown Stencil format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetSizedFormat(UnsizedFormat unsizedFormat, Type type, bool normalized)
{
    switch (unsizedFormat) {
    case UnsizedFormat::R:
        return GetRSizedformat(type, normalized);
    case UnsizedFormat::RG:
        return GetRGSizedformat(type, normalized);
    case UnsizedFormat::RGB:
        return GetRGBSizedformat(type, normalized);
    case UnsizedFormat::RGBA:
        return GetRGBASizedFormat(type, normalized);
    case UnsizedFormat::Depth:
        return GetDepthSizedformat(type, normalized);
    case UnsizedFormat::Depth_Stencil:
        return GetDepthStencilSizedFormat(type, normalized);
    case UnsizedFormat::Stencil:
        return GetStencilSizedFormat(type, normalized);
    default:
        throw std::runtime_error("Unknown Pixel::UnsizedFormat/Type");
    }
}

uint8_t GetUnsizedFormatComponentsNbr(UnsizedFormat format)
{
    switch (format) {
    case UnsizedFormat::R:
    case UnsizedFormat::R_Integer:
    case UnsizedFormat::Depth:
    case UnsizedFormat::Stencil:
        return 1;
    case UnsizedFormat::RG:
    case UnsizedFormat::RG_Integer:
    case UnsizedFormat::Depth_Stencil:
        return 2;
    case UnsizedFormat::RGB:
    case UnsizedFormat::RGB_Integer:
        return 3;
    case UnsizedFormat::RGBA:
    case UnsizedFormat::RGBA_Integer:
        return 4;
    default:
        throw std::runtime_error("Unknown Pixel::UnsizedFormat");
    }
}

uint8_t GetTypeSize(Type type)
{
    switch (type) {
    case Type::Uint8:
    case Type::Int8:
        return 1;
    case Type::Uint16:
    case Type::Int16:
    case Type::Float16:
        return 2;
    case Type::Uint32:
    case Type::Int32:
    case Type::Float32:
        return 4;
    case Type::DXT5Block:
        return 16;
    default:
        throw std::runtime_error("Unknown Pixel::Type");
    }
}

uint8_t GetOctetsPerPixels(UnsizedFormat format, Type Type)
{
    return GetUnsizedFormatComponentsNbr(format) * GetTypeSize(Type);
}

Description::Description(UnsizedFormat format, Type type, bool normalized)
    : Description(Pixel::GetSizedFormat(format, type, normalized))
{
}

Description::Description(SizedFormat format)
{
#ifndef NDEBUG
    assert(format != SizedFormat::Unknown);
#endif
    switch (format) {
    case SizedFormat::Uint8_NormalizedR:
        _UnsizedFormat = UnsizedFormat::R;
        _Type          = Type::Uint8;
        _Normalized    = true;
        break;
    case SizedFormat::Uint8_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _Type          = Type::Uint8;
        _Normalized    = true;
        break;
    case SizedFormat::Uint8_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _Type          = Type::Uint8;
        _Normalized    = true;
        break;
    case SizedFormat::Uint8_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _Type          = Type::Uint8;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int8_NormalizedR:
        _UnsizedFormat = UnsizedFormat::R;
        _Type          = Type::Int8;
        _Normalized    = true;
        break;
    case SizedFormat::Int8_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _Type          = Type::Int8;
        _Normalized    = true;
        break;
    case SizedFormat::Int8_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _Type          = Type::Int8;
        _Normalized    = true;
        break;
    case SizedFormat::Int8_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _Type          = Type::Int8;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint8_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _Type          = Type::Uint8;
        break;
    case SizedFormat::Uint8_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _Type          = Type::Uint8;
        break;
    case SizedFormat::Uint8_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _Type          = Type::Uint8;
        break;
    case SizedFormat::Uint8_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _Type          = Type::Uint8;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int8_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _Type          = Type::Int8;
        break;
    case SizedFormat::Int8_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _Type          = Type::Int8;
        break;
    case SizedFormat::Int8_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _Type          = Type::Int8;
        break;
    case SizedFormat::Int8_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _Type          = Type::Int8;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint16_NormalizedR:
        _UnsizedFormat = UnsizedFormat::R;
        _Type          = Type::Uint16;
        _Normalized    = true;
        break;
    case SizedFormat::Uint16_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _Type          = Type::Uint16;
        _Normalized    = true;
        break;
    case SizedFormat::Uint16_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _Type          = Type::Uint16;
        _Normalized    = true;
        break;
    case SizedFormat::Uint16_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _Type          = Type::Uint16;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int16_NormalizedR:
        _UnsizedFormat = UnsizedFormat::R;
        _Type          = Type::Int16;
        _Normalized    = true;
        break;
    case SizedFormat::Int16_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _Type          = Type::Int16;
        _Normalized    = true;
        break;
    case SizedFormat::Int16_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _Type          = Type::Int16;
        _Normalized    = true;
        break;
    case SizedFormat::Int16_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _Type          = Type::Int16;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint16_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _Type          = Type::Uint16;
        break;
    case SizedFormat::Uint16_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _Type          = Type::Uint16;
        break;
    case SizedFormat::Uint16_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _Type          = Type::Uint16;
        break;
    case SizedFormat::Uint16_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _Type          = Type::Uint16;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int16_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _Type          = Type::Int16;
        break;
    case SizedFormat::Int16_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _Type          = Type::Int16;
        break;
    case SizedFormat::Int16_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _Type          = Type::Int16;
        break;
    case SizedFormat::Int16_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _Type          = Type::Int16;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint32_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _Type          = Type::Uint32;
        break;
    case SizedFormat::Uint32_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _Type          = Type::Uint32;
        break;
    case SizedFormat::Uint32_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _Type          = Type::Uint32;
        break;
    case SizedFormat::Uint32_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _Type          = Type::Uint32;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int32_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _Type          = Type::Int32;
        break;
    case SizedFormat::Int32_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _Type          = Type::Int32;
        break;
    case SizedFormat::Int32_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _Type          = Type::Int32;
        break;
    case SizedFormat::Int32_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _Type          = Type::Int32;
        _HasAlpha      = true;
        break;
    case SizedFormat::Float16_R:
        _UnsizedFormat = UnsizedFormat::R;
        _Type          = Type::Float16;
        break;
    case SizedFormat::Float16_RG:
        _UnsizedFormat = UnsizedFormat::RG;
        _Type          = Type::Float16;
        break;
    case SizedFormat::Float16_RGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _Type          = Type::Float16;
        break;
    case SizedFormat::Float16_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _Type          = Type::Float16;
        _HasAlpha      = true;
        break;
    case SizedFormat::Float32_R:
        _UnsizedFormat = UnsizedFormat::R;
        _Type          = Type::Float32;
        break;
    case SizedFormat::Float32_RG:
        _UnsizedFormat = UnsizedFormat::RG;
        _Type          = Type::Float32;
        break;
    case SizedFormat::Float32_RGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _Type          = Type::Float32;
        break;
    case SizedFormat::Float32_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _Type          = Type::Float32;
        _HasAlpha      = true;
        break;
    case SizedFormat::Depth16:
        _UnsizedFormat = UnsizedFormat::Depth;
        _Type          = Type::Float16;
        break;
    case SizedFormat::Depth24:
    case SizedFormat::Depth32F:
    case SizedFormat::Depth32:
        _UnsizedFormat = UnsizedFormat::Depth;
        _Type          = Type::Float32;
        break;
    case SizedFormat::Depth24_Stencil8:
        _UnsizedFormat = UnsizedFormat::Depth;
        _Type          = Type::Uint32;
        break;
    case SizedFormat::Depth32F_Stencil8:
        _UnsizedFormat = UnsizedFormat::Depth_Stencil;
        _Type          = Type::Uint32;
        break;
    case SizedFormat::Stencil8:
        _UnsizedFormat = UnsizedFormat::Stencil;
        _Type          = Type::Uint8;
        break;
    case SizedFormat::DXT5_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _Type          = Type::DXT5Block;
        break;
    default:
        throw std::runtime_error("Unknown Format");
    }
    _SetSizedFormat(format);
    _SetTypeSize(Pixel::GetTypeSize(GetType()));
    _SetComponents(GetUnsizedFormatComponentsNbr(GetUnsizedFormat()));
    _SetSize(GetComponents() * GetTypeSize());
}

Color LinearToSRGB(const Color& color)
{
    const glm::vec3 linearRGB { color.r, color.g, color.b };
    glm::bvec3 cutoff = lessThan(linearRGB, glm::vec3(0.0031308));
    glm::vec3 higher  = glm::vec3(1.055) * pow(linearRGB, glm::vec3(1.0 / 2.4)) - glm::vec3(0.055);
    glm::vec3 lower   = linearRGB * glm::vec3(12.92);

    return Color(mix(higher, lower, cutoff), color.a);
}

float GetNormalizedColorComponent(Type type, const std::byte* bytes)
{
#ifndef NDEBUG
    assert(type != Type::Unknown);
    assert(type != Type::Uint32 && "Uint32 textures cannot be normalized");
    assert(type != Type::Int32 && "Int32 textures cannot be normalized");
    assert(type != Type::Float16 && "Float16 textures cannot be normalized");
    assert(type != Type::Float32 && "Float32 textures cannot be normalized");
#endif
    switch (type) {
    case Type::Uint8:
        return *reinterpret_cast<const uint8_t*>(bytes) / float(UINT8_MAX);
    case Type::Int8:
        return *reinterpret_cast<const int8_t*>(bytes) / float(INT8_MAX);
    case Type::Uint16:
        return *reinterpret_cast<const uint16_t*>(bytes) / float(UINT16_MAX);
    case Type::Int16:
        return *reinterpret_cast<const int16_t*>(bytes) / float(INT16_MAX);
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0;
}

float GetColorComponent(Type type, const std::byte* bytes)
{
#ifndef NDEBUG
    assert(type != Type::Unknown);
#endif
    switch (type) {
    case Type::Uint8:
        return *reinterpret_cast<const uint8_t*>(bytes);
    case Type::Int8:
        return *reinterpret_cast<const int8_t*>(bytes);
    case Type::Uint16:
        return *reinterpret_cast<const uint16_t*>(bytes);
    case Type::Int16:
        return *reinterpret_cast<const int16_t*>(bytes);
    case Type::Uint32:
        return *reinterpret_cast<const uint32_t*>(bytes);
    case Type::Int32:
        return *reinterpret_cast<const int32_t*>(bytes);
    case Type::Float16:
        return glm::detail::toFloat32(*reinterpret_cast<const glm::detail::hdata*>(bytes));
    case Type::Float32:
        return *reinterpret_cast<const float*>(bytes);
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0;
}

Color Description::GetColorFromBytes(const std::vector<std::byte>& bytes, const Size& imageSize, const Size& pixelCoordinates) const
{
    auto pixelIndex { GetPixelIndex(imageSize, pixelCoordinates) };
    auto pixelPtr { &bytes.at(pixelIndex) };
    assert((pixelIndex + GetSize()) <= bytes.size() && "The pixel is out of bound");
    return GetColorFromBytes(pixelPtr);
}

Color Description::GetColorFromBytes(const std::byte* bytes) const
{
    Color color { 0, 0, 0, 1 };
    auto getComponent = GetNormalized() ? &GetNormalizedColorComponent : &GetColorComponent;
    if (GetComponents() > 4)
        throw std::runtime_error("Incorrect pixel type");
    for (unsigned i = 0; i < GetComponents(); ++i) {
        color[i] = getComponent(GetType(), &bytes[GetTypeSize() * i]);
    }
    return color;
}

static inline void SetComponentNormalized(Type type, std::byte* bytes, float component)
{
#ifndef NDEBUG
    assert(type != Type::Unknown);
    assert(type != Type::Uint32 && "Uint32 textures cannot be normalized");
    assert(type != Type::Int32 && "Int32 textures cannot be normalized");
    assert(type != Type::Float16 && "Float16 textures cannot be normalized");
    assert(type != Type::Float32 && "Float32 textures cannot be normalized");
#endif
    switch (type) {
    case Type::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = glm::clamp(component, 0.f, 1.f) * float(UINT8_MAX);
        break;
    case Type::Int8:
        *reinterpret_cast<int8_t*>(bytes) = glm::clamp(component, -1.f, 1.f) * float(INT8_MAX);
        break;
    case Type::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = glm::clamp(component, 0.f, 1.f) * float(UINT16_MAX);
        break;
    case Type::Int16:
        *reinterpret_cast<int16_t*>(bytes) = glm::clamp(component, -1.f, 1.f) * float(INT16_MAX);
        break;
    default:
        throw std::runtime_error("Cannot set color for this pixel type");
    }
}

static inline void SetComponent(Type type, std::byte* bytes, float component)
{
#ifndef NDEBUG
    assert(type != Type::Unknown);
#endif
    switch (type) {
    case Type::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = component;
        break;
    case Type::Int8:
        *reinterpret_cast<int8_t*>(bytes) = component;
        break;
    case Type::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = component;
        break;
    case Type::Int16:
        *reinterpret_cast<int16_t*>(bytes) = component;
        break;
    case Type::Uint32:
        *reinterpret_cast<uint32_t*>(bytes) = component;
        break;
    case Type::Int32:
        *reinterpret_cast<int32_t*>(bytes) = component;
        break;
    case Type::Float16:
        *reinterpret_cast<glm::detail::hdata*>(bytes) = glm::detail::toFloat16(component);
        break;
    case Type::Float32:
        *reinterpret_cast<float*>(bytes) = component;
        break;
    default:
        throw std::runtime_error("Cannot set color for this pixel type");
    }
}

void Description::SetColorToBytes(std::byte* bytes, const Color& color) const
{
    auto setComponent = GetNormalized() ? SetComponentNormalized : SetComponent;
    if (GetComponents() > 4)
        throw std::runtime_error("Incorrect pixel type");
    for (unsigned i = 0; i < GetComponents(); ++i) {
        setComponent(GetType(), &bytes[GetTypeSize() * i], color[i]);
    }
}
}
