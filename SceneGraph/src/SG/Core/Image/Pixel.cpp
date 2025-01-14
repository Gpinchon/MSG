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
SizedFormat GetSizedFormat(UnsizedFormat unsizedFormat, DataType a_DataType)
{
    return SizedFormat((uint32_t(a_DataType) << 16) | uint32_t(unsizedFormat));
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

uint8_t GetChannelOctets(const SizedFormat& a_Format, const ColorChannel& a_Channel)
{
    auto dataType = DataType(uint32_t(a_Format) >> 16);
    return DataTypeSize(dataType) * (uint32_t(a_Format) & uint32_t(a_Channel));
}

Description::Description(UnsizedFormat format, DataType a_Type)
    : Description(Pixel::GetSizedFormat(format, a_Type))
{
}

Description::Description(SizedFormat a_Format)
{
    assert(a_Format <= SizedFormat::MaxValue);
    _SizedFormat   = a_Format;
    _DataType      = DataType(uint32_t(_SizedFormat) >> 16);
    _TypeSize      = DataTypeSize(_DataType);
    _UnsizedFormat = UnsizedFormat(uint32_t(_SizedFormat) & uint32_t(UnsizedFormat::MaxValue));
    _Normalized    = (uint16_t(_UnsizedFormat) >> 8) & PixelTypeNormalized;
    _HasAlpha      = uint32_t(_UnsizedFormat) & ColorChannelAlpha;
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
