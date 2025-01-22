#include <Core/Image/Pixel.hpp>
#include <Core/Image/PixelSizedFormatHelper.hpp>

#include <bit>
#include <glm/glm.hpp>
#include <stdexcept>

namespace MSG::Core::Pixel {
uint8_t GetChannelDataTypeSize(const SizedFormat& a_Format, const ColorChannel& a_Channel)
{
    assert(a_Format != SizedFormat::Unknown);
    if (a_Format == SizedFormat::None)
        return 0;
    auto helper = SizedFormatHelper(a_Format);
    if (helper.format == UnsizedFormat::Depth || helper.format == UnsizedFormat::Stencil || helper.format == UnsizedFormat::Depth_Stencil) {
        assert(a_Channel == ColorChannelDepth || a_Channel == ColorChannelStencil);
        return DataTypeSize(a_Channel == ColorChannelDepth ? helper.depth : helper.stencil);
    }
    return DataTypeSize(helper.channel[GetChannelIndex(a_Channel)]);
}

Color LinearToSRGB(const Color& color)
{
    const auto linearRGB = glm::vec3(color);
    const auto cutoff    = lessThan(linearRGB, glm::vec3(0.0031308f));
    const auto higher    = glm::vec3(1.055f) * pow(linearRGB, glm::vec3(1.f / 2.4f)) - glm::vec3(0.055f);
    const auto lower     = linearRGB * glm::vec3(12.92f);
    return Color(mix(higher, lower, cutoff), color.a);
}

float GetNormalizedColorComponent(const DataType& a_DataType, const std::byte* a_Bytes)
{
    assert(a_DataType != DataType::Unknown);
    assert(a_DataType != DataType::Uint32 && "Uint32 textures cannot be normalized");
    assert(a_DataType != DataType::Int32 && "Int32 textures cannot be normalized");
    assert(a_DataType != DataType::Float16 && "Float16 textures cannot be normalized");
    assert(a_DataType != DataType::Float32 && "Float32 textures cannot be normalized");
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

float GetColorComponent(const DataType& a_DataType, const std::byte* a_Bytes)
{
    assert(a_DataType != DataType::Unknown);
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

Description::Description(const SizedFormat& a_Format)
    : _SizedFormatHelper(a_Format)
{
    for (uint8_t i = 0; i < 4; i++)
        _Size += GetDataTypeSize(i);
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
    auto getComponent = IsNormalized() ? &GetNormalizedColorComponent : &GetColorComponent;
    if (GetComponentsNbr() > 4)
        throw std::runtime_error("Incorrect pixel type");
    for (uint8_t i = 0, offset = 0; i < GetComponentsNbr(); ++i) {
        color[i] = getComponent(GetDataType(i), &bytes[offset]);
        offset += GetDataTypeSize(i);
    }
    return color;
}

static inline void SetComponentNormalized(DataType a_DataType, std::byte* bytes, float component)
{
    assert(a_DataType != DataType::Unknown);
    assert(a_DataType != DataType::Uint32 && "Uint32 textures cannot be normalized");
    assert(a_DataType != DataType::Int32 && "Int32 textures cannot be normalized");
    assert(a_DataType != DataType::Float16 && "Float16 textures cannot be normalized");
    assert(a_DataType != DataType::Float32 && "Float32 textures cannot be normalized");
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
    assert(a_DataType != DataType::Unknown);
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
    auto setComponent = IsNormalized() ? SetComponentNormalized : SetComponent;
    assert(GetComponentsNbr() <= 4);
    for (unsigned i = 0, offset = 0; i < GetComponentsNbr(); ++i) {
        setComponent(GetDataType(i), &bytes[offset], color[i]);
        offset += GetDataTypeSize(i);
    }
}
}
