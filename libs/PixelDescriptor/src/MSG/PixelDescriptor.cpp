#include <MSG/Pixel/SizedFormatHelper.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <bit>
#include <glm/glm.hpp>
#include <stdexcept>

namespace MSG {
uint8_t GetPixelChannelDataTypeSize(const PixelSizedFormat& a_Format, const PixelColorChannel& a_Channel)
{
    assert(a_Format != PixelSizedFormat::Unknown);
    if (a_Format == PixelSizedFormat::None)
        return 0;
    auto helper = PixelSizedFormatHelper(a_Format);
    if (helper.format == PixelUnsizedFormat::Depth || helper.format == PixelUnsizedFormat::Stencil || helper.format == PixelUnsizedFormat::Depth_Stencil) {
        assert(a_Channel == PixelColorChannelDepth || a_Channel == PixelColorChannelStencil);
        return Core::DataTypeSize(a_Channel == PixelColorChannelDepth ? helper.depth : helper.stencil);
    }
    return Core::DataTypeSize(helper.channel[GetPixelChannelIndex(a_Channel)]);
}

PixelColor LinearToSRGB(const PixelColor& color)
{
    const auto linearRGB = glm::vec3(color);
    const auto cutoff    = lessThan(linearRGB, glm::vec3(0.0031308f));
    const auto higher    = glm::vec3(1.055f) * pow(linearRGB, glm::vec3(1.f / 2.4f)) - glm::vec3(0.055f);
    const auto lower     = linearRGB * glm::vec3(12.92f);
    return PixelColor(mix(higher, lower, cutoff), color.a);
}

float GetNormalizedColorComponent(const Core::DataType& a_DataType, const std::byte* a_Bytes)
{
    assert(a_DataType != Core::DataType::Unknown);
    assert(a_DataType != Core::DataType::Uint32 && "Uint32 textures cannot be normalized");
    assert(a_DataType != Core::DataType::Int32 && "Int32 textures cannot be normalized");
    assert(a_DataType != Core::DataType::Float16 && "Float16 textures cannot be normalized");
    assert(a_DataType != Core::DataType::Float32 && "Float32 textures cannot be normalized");
    switch (a_DataType) {
    case Core::DataType::Uint8:
        return *reinterpret_cast<const uint8_t*>(a_Bytes) / float(UINT8_MAX);
    case Core::DataType::Int8:
        return *reinterpret_cast<const int8_t*>(a_Bytes) / float(INT8_MAX);
    case Core::DataType::Uint16:
        return *reinterpret_cast<const uint16_t*>(a_Bytes) / float(UINT16_MAX);
    case Core::DataType::Int16:
        return *reinterpret_cast<const int16_t*>(a_Bytes) / float(INT16_MAX);
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0;
}

float GetColorComponent(const Core::DataType& a_DataType, const std::byte* a_Bytes)
{
    assert(a_DataType != Core::DataType::Unknown);
    switch (a_DataType) {
    case Core::DataType::Uint8:
        return float(*reinterpret_cast<const uint8_t*>(a_Bytes));
    case Core::DataType::Int8:
        return float(*reinterpret_cast<const int8_t*>(a_Bytes));
    case Core::DataType::Uint16:
        return float(*reinterpret_cast<const uint16_t*>(a_Bytes));
    case Core::DataType::Int16:
        return float(*reinterpret_cast<const int16_t*>(a_Bytes));
    case Core::DataType::Uint32:
        return float(*reinterpret_cast<const uint32_t*>(a_Bytes));
    case Core::DataType::Int32:
        return float(*reinterpret_cast<const int32_t*>(a_Bytes));
    case Core::DataType::Float16:
        return float(glm::detail::toFloat32(*reinterpret_cast<const glm::detail::hdata*>(a_Bytes)));
    case Core::DataType::Float32:
        return float(*reinterpret_cast<const float*>(a_Bytes));
    default:
        throw std::runtime_error("Cannot fetch color for this pixel type");
    }
    return 0.f;
}

PixelDescriptor::PixelDescriptor(const PixelSizedFormat& a_Format)
    : _PixelSizedFormatHelper(a_Format)
{
    for (uint8_t i = 0; i < 4; i++)
        _PixelSize += GetDataTypeSize(i);
}

PixelColor PixelDescriptor::GetColorFromBytes(const std::vector<std::byte>& bytes, const PixelSize& imageSize, const PixelSize& pixelCoordinates) const
{
    auto pixelIndex { GetPixelIndex(imageSize, pixelCoordinates) };
    auto pixelPtr { &bytes.at(pixelIndex) };
    assert((pixelIndex + GetPixelSize()) <= bytes.size() && "The pixel is out of bound");
    return GetColorFromBytes(pixelPtr);
}

PixelColor PixelDescriptor::GetColorFromBytes(const std::byte* bytes) const
{
    assert(GetComponentsNbr() <= 4 && "Incorrect pixel type");
    PixelColor color { 0, 0, 0, 1 };
    auto getComponent = IsNormalized() ? &GetNormalizedColorComponent : &GetColorComponent;
    for (uint8_t i = 0, offset = 0; i < GetComponentsNbr(); ++i) {
        color[i] = getComponent(GetDataType(i), &bytes[offset]);
        offset += GetDataTypeSize(i);
    }
    return color;
}

static inline void SetComponentNormalized(const Core::DataType& a_DataType, std::byte* bytes, float component)
{
    assert(a_DataType != Core::DataType::Unknown);
    assert(a_DataType != Core::DataType::Uint32 && "Uint32 textures cannot be normalized");
    assert(a_DataType != Core::DataType::Int32 && "Int32 textures cannot be normalized");
    assert(a_DataType != Core::DataType::Float16 && "Float16 textures cannot be normalized");
    assert(a_DataType != Core::DataType::Float32 && "Float32 textures cannot be normalized");
    switch (a_DataType) {
    case Core::DataType::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = uint8_t(glm::clamp(component, 0.f, 1.f) * float(UINT8_MAX));
        break;
    case Core::DataType::Int8:
        *reinterpret_cast<int8_t*>(bytes) = int8_t(glm::clamp(component, -1.f, 1.f) * float(INT8_MAX));
        break;
    case Core::DataType::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = uint16_t(glm::clamp(component, 0.f, 1.f) * float(UINT16_MAX));
        break;
    case Core::DataType::Int16:
        *reinterpret_cast<int16_t*>(bytes) = int16_t(glm::clamp(component, -1.f, 1.f) * float(INT16_MAX));
        break;
    default:
        throw std::runtime_error("Cannot set color for this pixel type");
    }
}

static inline void SetComponent(const Core::DataType& a_DataType, std::byte* bytes, float component)
{
    assert(a_DataType != Core::DataType::Unknown);
    switch (a_DataType) {
    case Core::DataType::Uint8:
        *reinterpret_cast<uint8_t*>(bytes) = uint8_t(component);
        break;
    case Core::DataType::Int8:
        *reinterpret_cast<int8_t*>(bytes) = int8_t(component);
        break;
    case Core::DataType::Uint16:
        *reinterpret_cast<uint16_t*>(bytes) = uint16_t(component);
        break;
    case Core::DataType::Int16:
        *reinterpret_cast<int16_t*>(bytes) = int16_t(component);
        break;
    case Core::DataType::Uint32:
        *reinterpret_cast<uint32_t*>(bytes) = uint32_t(component);
        break;
    case Core::DataType::Int32:
        *reinterpret_cast<int32_t*>(bytes) = int32_t(component);
        break;
    case Core::DataType::Float16:
        *reinterpret_cast<glm::detail::hdata*>(bytes) = glm::detail::toFloat16(component);
        break;
    case Core::DataType::Float32:
        *reinterpret_cast<float*>(bytes) = component;
        break;
    default:
        throw std::runtime_error("Cannot set color for this pixel type");
    }
}

void PixelDescriptor::SetColorToBytes(std::byte* bytes, const PixelColor& color) const
{
    auto setComponent = IsNormalized() ? SetComponentNormalized : SetComponent;
    assert(GetComponentsNbr() <= 4);
    for (unsigned i = 0, offset = 0; i < GetComponentsNbr(); ++i) {
        setComponent(GetDataType(i), &bytes[offset], color[i]);
        offset += GetDataTypeSize(i);
    }
}
}
