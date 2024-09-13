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
inline auto& GetSizedFormatLUT()
{
    constexpr auto ComponentsNbr1 = 0;
    constexpr auto ComponentsNbr2 = 1;
    constexpr auto ComponentsNbr3 = 2;
    constexpr auto ComponentsNbr4 = 3;
    constexpr auto ComponentsMax  = 4;
    constexpr auto Unnormalized   = 0;
    constexpr auto Normalized     = 1;
    static bool s_Initialized     = false;
    static SizedFormat s_SizedFormatLUT[ComponentsMax][2][int(DataType::MaxValue)];
    if (s_Initialized)
        return s_SizedFormatLUT;
    for (auto comp = 0; comp < ComponentsMax; comp++) {
        for (auto type = 0; type < int(DataType::MaxValue); type++) {
            s_SizedFormatLUT[ComponentsNbr1][Normalized][type]   = SizedFormat::Unknown;
            s_SizedFormatLUT[ComponentsNbr1][Unnormalized][type] = SizedFormat::Unknown;
        }
    }
    s_SizedFormatLUT[ComponentsNbr1][Normalized][int(DataType::Uint8)]     = SizedFormat::Uint8_NormalizedR;
    s_SizedFormatLUT[ComponentsNbr1][Normalized][int(DataType::Int8)]      = SizedFormat::Int8_NormalizedR;
    s_SizedFormatLUT[ComponentsNbr1][Normalized][int(DataType::Uint16)]    = SizedFormat::Uint16_NormalizedR;
    s_SizedFormatLUT[ComponentsNbr1][Normalized][int(DataType::Int16)]     = SizedFormat::Int16_NormalizedR;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Uint8)]   = SizedFormat::Uint8_R;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Int8)]    = SizedFormat::Int8_R;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Uint16)]  = SizedFormat::Uint16_R;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Int16)]   = SizedFormat::Int16_R;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Uint32)]  = SizedFormat::Uint32_R;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Int32)]   = SizedFormat::Int32_R;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Float16)] = SizedFormat::Float16_R;
    s_SizedFormatLUT[ComponentsNbr1][Unnormalized][int(DataType::Float32)] = SizedFormat::Float32_R;

    s_SizedFormatLUT[ComponentsNbr2][Normalized][int(DataType::Uint8)]     = SizedFormat::Uint8_NormalizedRG;
    s_SizedFormatLUT[ComponentsNbr2][Normalized][int(DataType::Int8)]      = SizedFormat::Int8_NormalizedRG;
    s_SizedFormatLUT[ComponentsNbr2][Normalized][int(DataType::Uint16)]    = SizedFormat::Uint16_NormalizedRG;
    s_SizedFormatLUT[ComponentsNbr2][Normalized][int(DataType::Int16)]     = SizedFormat::Int16_NormalizedRG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Uint8)]   = SizedFormat::Uint8_RG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Int8)]    = SizedFormat::Int8_RG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Uint16)]  = SizedFormat::Uint16_RG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Int16)]   = SizedFormat::Int16_RG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Uint32)]  = SizedFormat::Uint32_RG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Int32)]   = SizedFormat::Int32_RG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Float16)] = SizedFormat::Float16_RG;
    s_SizedFormatLUT[ComponentsNbr2][Unnormalized][int(DataType::Float32)] = SizedFormat::Float32_RG;

    s_SizedFormatLUT[ComponentsNbr3][Normalized][int(DataType::Uint8)]     = SizedFormat::Uint8_NormalizedRGB;
    s_SizedFormatLUT[ComponentsNbr3][Normalized][int(DataType::Int8)]      = SizedFormat::Int8_NormalizedRGB;
    s_SizedFormatLUT[ComponentsNbr3][Normalized][int(DataType::Uint16)]    = SizedFormat::Uint16_NormalizedRGB;
    s_SizedFormatLUT[ComponentsNbr3][Normalized][int(DataType::Int16)]     = SizedFormat::Int16_NormalizedRGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Uint8)]   = SizedFormat::Uint8_RGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Int8)]    = SizedFormat::Int8_RGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Uint16)]  = SizedFormat::Uint16_RGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Int16)]   = SizedFormat::Int16_RGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Uint32)]  = SizedFormat::Uint32_RGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Int32)]   = SizedFormat::Int32_RGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Float16)] = SizedFormat::Float16_RGB;
    s_SizedFormatLUT[ComponentsNbr3][Unnormalized][int(DataType::Float32)] = SizedFormat::Float32_RGB;

    s_SizedFormatLUT[ComponentsNbr4][Normalized][int(DataType::Uint8)]     = SizedFormat::Uint8_NormalizedRGBA;
    s_SizedFormatLUT[ComponentsNbr4][Normalized][int(DataType::Int8)]      = SizedFormat::Int8_NormalizedRGBA;
    s_SizedFormatLUT[ComponentsNbr4][Normalized][int(DataType::Uint16)]    = SizedFormat::Uint16_NormalizedRGBA;
    s_SizedFormatLUT[ComponentsNbr4][Normalized][int(DataType::Int16)]     = SizedFormat::Int16_NormalizedRGBA;
    s_SizedFormatLUT[ComponentsNbr4][Normalized][int(DataType::DXT5Block)] = SizedFormat::DXT5_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Uint8)]   = SizedFormat::Uint8_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Int8)]    = SizedFormat::Int8_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Uint16)]  = SizedFormat::Uint16_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Int16)]   = SizedFormat::Int16_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Uint32)]  = SizedFormat::Uint32_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Int32)]   = SizedFormat::Int32_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Float16)] = SizedFormat::Float16_RGBA;
    s_SizedFormatLUT[ComponentsNbr4][Unnormalized][int(DataType::Float32)] = SizedFormat::Float32_RGBA;

    return s_SizedFormatLUT;
}

template <uint8_t ComponentsNbr, bool normalized>
inline SizedFormat GetSizedformat(const DataType& a_Type)
{
    auto sizedFormat = GetSizedFormatLUT()[ComponentsNbr - 1][normalized][int(a_Type)];
    if (sizedFormat == SizedFormat::Unknown)
        throw std::runtime_error("Incorrect Pixel Type");
    return sizedFormat;
}

SizedFormat GetDepthSizedformat(DataType a_DataType)
{
    switch (a_DataType) {
    case DataType::Uint32:
        return SizedFormat::Depth32;
    case DataType::Float16:
        return SizedFormat::Depth16;
    case DataType::Float32:
        return SizedFormat::Depth32F;
    default:
        throw std::runtime_error("Incorrect Depth format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetDepthStencilSizedFormat(DataType a_DataType)
{
    switch (a_DataType) {
    case DataType::Uint32:
        return SizedFormat::Depth24_Stencil8;
        break;
    case DataType::Float32:
        return SizedFormat::Depth32F_Stencil8;
        break;
    default:
        throw std::runtime_error("Incorrect DepthStencil format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetStencilSizedFormat(DataType a_DataType)
{
    switch (a_DataType) {
    case DataType::Uint8:
        return SizedFormat::Stencil8;
        break;
    default:
        throw std::runtime_error("Incorrect Stencil format");
    }
    return SizedFormat::Unknown;
}

SizedFormat GetSizedFormat(UnsizedFormat unsizedFormat, DataType a_DataType)
{
    switch (unsizedFormat) {
    case UnsizedFormat::R:
        return GetSizedformat<1, true>(a_DataType);
    case UnsizedFormat::RG:
        return GetSizedformat<2, true>(a_DataType);
    case UnsizedFormat::RGB:
        return GetSizedformat<3, true>(a_DataType);
    case UnsizedFormat::RGBA:
        return GetSizedformat<4, true>(a_DataType);
    case UnsizedFormat::R_Integer:
        return GetSizedformat<1, false>(a_DataType);
    case UnsizedFormat::RG_Integer:
        return GetSizedformat<2, false>(a_DataType);
    case UnsizedFormat::RGB_Integer:
        return GetSizedformat<3, false>(a_DataType);
    case UnsizedFormat::RGBA_Integer:
        return GetSizedformat<4, false>(a_DataType);
    case UnsizedFormat::Depth:
        return GetDepthSizedformat(a_DataType);
    case UnsizedFormat::Depth_Stencil:
        return GetDepthStencilSizedFormat(a_DataType);
    case UnsizedFormat::Stencil:
        return GetStencilSizedFormat(a_DataType);
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

uint8_t GetOctetsPerPixels(UnsizedFormat format, DataType a_DataType)
{
    return GetUnsizedFormatComponentsNbr(format) * DataTypeSize(a_DataType);
}

Description::Description(UnsizedFormat format, DataType type)
    : Description(Pixel::GetSizedFormat(format, type))
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
        _DataType      = DataType::Uint8;
        _Normalized    = true;
        break;
    case SizedFormat::Uint8_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _DataType      = DataType::Uint8;
        _Normalized    = true;
        break;
    case SizedFormat::Uint8_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _DataType      = DataType::Uint8;
        _Normalized    = true;
        break;
    case SizedFormat::Uint8_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _DataType      = DataType::Uint8;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int8_NormalizedR:
        _UnsizedFormat = UnsizedFormat::R;
        _DataType      = DataType::Int8;
        _Normalized    = true;
        break;
    case SizedFormat::Int8_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _DataType      = DataType::Int8;
        _Normalized    = true;
        break;
    case SizedFormat::Int8_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _DataType      = DataType::Int8;
        _Normalized    = true;
        break;
    case SizedFormat::Int8_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _DataType      = DataType::Int8;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint8_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _DataType      = DataType::Uint8;
        break;
    case SizedFormat::Uint8_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _DataType      = DataType::Uint8;
        break;
    case SizedFormat::Uint8_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _DataType      = DataType::Uint8;
        break;
    case SizedFormat::Uint8_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _DataType      = DataType::Uint8;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int8_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _DataType      = DataType::Int8;
        break;
    case SizedFormat::Int8_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _DataType      = DataType::Int8;
        break;
    case SizedFormat::Int8_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _DataType      = DataType::Int8;
        break;
    case SizedFormat::Int8_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _DataType      = DataType::Int8;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint16_NormalizedR:
        _UnsizedFormat = UnsizedFormat::R;
        _DataType      = DataType::Uint16;
        _Normalized    = true;
        break;
    case SizedFormat::Uint16_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _DataType      = DataType::Uint16;
        _Normalized    = true;
        break;
    case SizedFormat::Uint16_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _DataType      = DataType::Uint16;
        _Normalized    = true;
        break;
    case SizedFormat::Uint16_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _DataType      = DataType::Uint16;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int16_NormalizedR:
        _UnsizedFormat = UnsizedFormat::R;
        _DataType      = DataType::Int16;
        _Normalized    = true;
        break;
    case SizedFormat::Int16_NormalizedRG:
        _UnsizedFormat = UnsizedFormat::RG;
        _DataType      = DataType::Int16;
        _Normalized    = true;
        break;
    case SizedFormat::Int16_NormalizedRGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _DataType      = DataType::Int16;
        _Normalized    = true;
        break;
    case SizedFormat::Int16_NormalizedRGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _DataType      = DataType::Int16;
        _Normalized    = true;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint16_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _DataType      = DataType::Uint16;
        break;
    case SizedFormat::Uint16_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _DataType      = DataType::Uint16;
        break;
    case SizedFormat::Uint16_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _DataType      = DataType::Uint16;
        break;
    case SizedFormat::Uint16_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _DataType      = DataType::Uint16;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int16_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _DataType      = DataType::Int16;
        break;
    case SizedFormat::Int16_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _DataType      = DataType::Int16;
        break;
    case SizedFormat::Int16_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _DataType      = DataType::Int16;
        break;
    case SizedFormat::Int16_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _DataType      = DataType::Int16;
        _HasAlpha      = true;
        break;
    case SizedFormat::Uint32_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _DataType      = DataType::Uint32;
        break;
    case SizedFormat::Uint32_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _DataType      = DataType::Uint32;
        break;
    case SizedFormat::Uint32_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _DataType      = DataType::Uint32;
        break;
    case SizedFormat::Uint32_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _DataType      = DataType::Uint32;
        _HasAlpha      = true;
        break;
    case SizedFormat::Int32_R:
        _UnsizedFormat = UnsizedFormat::R_Integer;
        _DataType      = DataType::Int32;
        break;
    case SizedFormat::Int32_RG:
        _UnsizedFormat = UnsizedFormat::RG_Integer;
        _DataType      = DataType::Int32;
        break;
    case SizedFormat::Int32_RGB:
        _UnsizedFormat = UnsizedFormat::RGB_Integer;
        _DataType      = DataType::Int32;
        break;
    case SizedFormat::Int32_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA_Integer;
        _DataType      = DataType::Int32;
        _HasAlpha      = true;
        break;
    case SizedFormat::Float16_R:
        _UnsizedFormat = UnsizedFormat::R;
        _DataType      = DataType::Float16;
        break;
    case SizedFormat::Float16_RG:
        _UnsizedFormat = UnsizedFormat::RG;
        _DataType      = DataType::Float16;
        break;
    case SizedFormat::Float16_RGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _DataType      = DataType::Float16;
        break;
    case SizedFormat::Float16_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _DataType      = DataType::Float16;
        _HasAlpha      = true;
        break;
    case SizedFormat::Float32_R:
        _UnsizedFormat = UnsizedFormat::R;
        _DataType      = DataType::Float32;
        break;
    case SizedFormat::Float32_RG:
        _UnsizedFormat = UnsizedFormat::RG;
        _DataType      = DataType::Float32;
        break;
    case SizedFormat::Float32_RGB:
        _UnsizedFormat = UnsizedFormat::RGB;
        _DataType      = DataType::Float32;
        break;
    case SizedFormat::Float32_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _DataType      = DataType::Float32;
        _HasAlpha      = true;
        break;
    case SizedFormat::Depth16:
        _UnsizedFormat = UnsizedFormat::Depth;
        _DataType      = DataType::Float16;
        break;
    case SizedFormat::Depth24:
    case SizedFormat::Depth32F:
    case SizedFormat::Depth32:
        _UnsizedFormat = UnsizedFormat::Depth;
        _DataType      = DataType::Float32;
        break;
    case SizedFormat::Depth24_Stencil8:
        _UnsizedFormat = UnsizedFormat::Depth;
        _DataType      = DataType::Uint32;
        break;
    case SizedFormat::Depth32F_Stencil8:
        _UnsizedFormat = UnsizedFormat::Depth_Stencil;
        _DataType      = DataType::Uint32;
        break;
    case SizedFormat::Stencil8:
        _UnsizedFormat = UnsizedFormat::Stencil;
        _DataType      = DataType::Uint8;
        break;
    case SizedFormat::DXT5_RGBA:
        _UnsizedFormat = UnsizedFormat::RGBA;
        _DataType      = DataType::DXT5Block;
        _HasAlpha      = true;
        _Normalized    = true;
        break;
    default:
        throw std::runtime_error("Unknown Format");
    }
    _SetSizedFormat(format);
    _SetTypeSize(DataTypeSize(GetDataType()));
    if (_DataType == DataType::DXT5Block)
        _SetComponents(1);
    else
        _SetComponents(GetUnsizedFormatComponentsNbr(GetUnsizedFormat()));
    _SetSize(GetComponents() * GetTypeSize());
}

Color LinearToSRGB(const Color& color)
{
    const auto linearRGB = glm::vec3(color);
    const auto cutoff    = lessThan(linearRGB, glm::vec3(0.0031308f));
    const auto higher    = glm::vec3(1.055f) * pow(linearRGB, glm::vec3(1.f / 2.4f)) - glm::vec3(0.055f);
    const auto lower     = linearRGB * glm::vec3(12.92f);
    return Color(mix(higher, lower, cutoff), color.a);
}

float GetNormalizedColorComponent(DataType a_DataType, const std::byte* a_Bytes)
{
#ifndef NDEBUG
    assert(a_DataType != DataType::Unknown);
    assert(a_DataType != DataType::Uint32 && "Uint32 textures cannot be normalized");
    assert(a_DataType != DataType::Int32 && "Int32 textures cannot be normalized");
    assert(a_DataType != DataType::Float16 && "Float16 textures cannot be normalized");
    assert(a_DataType != DataType::Float32 && "Float32 textures cannot be normalized");
#endif
    switch (a_DataType) {
    case DataType::Uint8:
        return *reinterpret_cast<const uint8_t*>(a_Bytes) / float(UINT8_MAX);
    case DataType::Int8:
        return *reinterpret_cast<const int8_t*>(a_Bytes) / float(INT8_MAX);
    case DataType::Uint16:
        return *reinterpret_cast<const uint16_t*>(a_Bytes) / float(UINT16_MAX);
    case DataType::Int16:
        return *reinterpret_cast<const int16_t*>(a_Bytes) / float(INT16_MAX);
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0;
}

float GetColorComponent(DataType a_DataType, const std::byte* a_Bytes)
{
#ifndef NDEBUG
    assert(a_DataType != DataType::Unknown);
#endif
    switch (a_DataType) {
    case DataType::Uint8:
        return float(*reinterpret_cast<const uint8_t*>(a_Bytes));
    case DataType::Int8:
        return float(*reinterpret_cast<const int8_t*>(a_Bytes));
    case DataType::Uint16:
        return float(*reinterpret_cast<const uint16_t*>(a_Bytes));
    case DataType::Int16:
        return float(*reinterpret_cast<const int16_t*>(a_Bytes));
    case DataType::Uint32:
        return float(*reinterpret_cast<const uint32_t*>(a_Bytes));
    case DataType::Int32:
        return float(*reinterpret_cast<const int32_t*>(a_Bytes));
    case DataType::Float16:
        return float(glm::detail::toFloat32(*reinterpret_cast<const glm::detail::hdata*>(a_Bytes)));
    case DataType::Float32:
        return float(*reinterpret_cast<const float*>(a_Bytes));
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0.f;
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
        color[i] = getComponent(GetDataType(), &bytes[GetTypeSize() * i]);
    }
    return color;
}

static inline void SetComponentNormalized(DataType a_DataType, std::byte* bytes, float component)
{
#ifndef NDEBUG
    assert(a_DataType != DataType::Unknown);
    assert(a_DataType != DataType::Uint32 && "Uint32 textures cannot be normalized");
    assert(a_DataType != DataType::Int32 && "Int32 textures cannot be normalized");
    assert(a_DataType != DataType::Float16 && "Float16 textures cannot be normalized");
    assert(a_DataType != DataType::Float32 && "Float32 textures cannot be normalized");
#endif
    switch (a_DataType) {
    case DataType::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = uint8_t(glm::clamp(component, 0.f, 1.f) * float(UINT8_MAX));
        break;
    case DataType::Int8:
        *reinterpret_cast<int8_t*>(bytes) = int8_t(glm::clamp(component, -1.f, 1.f) * float(INT8_MAX));
        break;
    case DataType::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = uint16_t(glm::clamp(component, 0.f, 1.f) * float(UINT16_MAX));
        break;
    case DataType::Int16:
        *reinterpret_cast<int16_t*>(bytes) = int16_t(glm::clamp(component, -1.f, 1.f) * float(INT16_MAX));
        break;
    default:
        throw std::runtime_error("Cannot set color for this pixel type");
    }
}

static inline void SetComponent(DataType a_DataType, std::byte* bytes, float component)
{
#ifndef NDEBUG
    assert(a_DataType != DataType::Unknown);
#endif
    switch (a_DataType) {
    case DataType::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = uint8_t(component);
        break;
    case DataType::Int8:
        *reinterpret_cast<int8_t*>(bytes) = int8_t(component);
        break;
    case DataType::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = uint16_t(component);
        break;
    case DataType::Int16:
        *reinterpret_cast<int16_t*>(bytes) = int16_t(component);
        break;
    case DataType::Uint32:
        *reinterpret_cast<uint32_t*>(bytes) = uint32_t(component);
        break;
    case DataType::Int32:
        *reinterpret_cast<int32_t*>(bytes) = int32_t(component);
        break;
    case DataType::Float16:
        *reinterpret_cast<glm::detail::hdata*>(bytes) = glm::detail::toFloat16(component);
        break;
    case DataType::Float32:
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
        setComponent(GetDataType(), &bytes[GetTypeSize() * i], color[i]);
    }
}
}
