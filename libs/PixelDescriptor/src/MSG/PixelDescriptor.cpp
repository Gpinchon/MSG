#include <MSG/Pixel/SizedFormatHelper.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <glm/glm.hpp>

#include <bit>

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

PixelColor LinearToSRGB(const PixelColor& a_Color)
{
    const auto linearRGB = glm::vec3(a_Color);
    const auto cutoff    = lessThan(linearRGB, glm::vec3(0.0031308f));
    const auto higher    = glm::vec3(1.055f) * pow(linearRGB, glm::vec3(1.f / 2.4f)) - glm::vec3(0.055f);
    const auto lower     = linearRGB * glm::vec3(12.92f);
    return PixelColor(mix(higher, lower, cutoff), a_Color.a);
}

float GetComponentNormalizedUint8(const std::byte* a_Bytes)
{
    return *reinterpret_cast<const uint8_t*>(a_Bytes) / float(UINT8_MAX);
}
float GetComponentNormalizedInt8(const std::byte* a_Bytes)
{
    return *reinterpret_cast<const int8_t*>(a_Bytes) / float(INT8_MAX);
}
float GetComponentNormalizedUint16(const std::byte* a_Bytes)
{
    return *reinterpret_cast<const uint16_t*>(a_Bytes) / float(UINT16_MAX);
}
float GetComponentNormalizedInt16(const std::byte* a_Bytes)
{
    return *reinterpret_cast<const int16_t*>(a_Bytes) / float(INT16_MAX);
}
float GetComponentUint8(const std::byte* a_Bytes)
{
    return float(*reinterpret_cast<const uint8_t*>(a_Bytes));
}
float GetComponentInt8(const std::byte* a_Bytes)
{
    return float(*reinterpret_cast<const int8_t*>(a_Bytes));
}
float GetComponentUint16(const std::byte* a_Bytes)
{
    return float(*reinterpret_cast<const uint16_t*>(a_Bytes));
}
float GetComponentInt16(const std::byte* a_Bytes)
{
    return float(*reinterpret_cast<const int16_t*>(a_Bytes));
}
float GetComponentUint32(const std::byte* a_Bytes)
{
    return float(*reinterpret_cast<const uint32_t*>(a_Bytes));
}
float GetComponentInt32(const std::byte* a_Bytes)
{
    return float(*reinterpret_cast<const int32_t*>(a_Bytes));
}
float GetComponentFloat16(const std::byte* a_Bytes)
{
    return float(glm::detail::toFloat32(*reinterpret_cast<const glm::detail::hdata*>(a_Bytes)));
}
float GetComponentFloat32(const std::byte* a_Bytes)
{
    return float(*reinterpret_cast<const float*>(a_Bytes));
}
auto GetGetComponentFunc(const PixelDescriptor& a_PD)
{
    std::array<PixelDescriptor::GetComponentFunc, 4> funcs;
    if (a_PD.IsNormalized()) {
        for (uint8_t i = 0; i < a_PD.GetComponentsNbr(); i++) {
            const auto dataType = a_PD.GetDataType(i);
            assert(dataType != Core::DataType::None);
            assert(dataType != Core::DataType::Uint32 && "Uint32 textures cannot be normalized");
            assert(dataType != Core::DataType::Int32 && "Int32 textures cannot be normalized");
            assert(dataType != Core::DataType::Float16 && "Float16 textures cannot be normalized");
            assert(dataType != Core::DataType::Float32 && "Float32 textures cannot be normalized");
            switch (dataType) {
            case Core::DataType::Uint8:
                funcs.at(i) = GetComponentNormalizedUint8;
                break;
            case Core::DataType::Int8:
                funcs.at(i) = GetComponentNormalizedInt8;
                break;
            case Core::DataType::Uint16:
                funcs.at(i) = GetComponentNormalizedUint16;
                break;
            case Core::DataType::Int16:
                funcs.at(i) = GetComponentNormalizedInt16;
                break;
            }
        }
    } else {
        for (uint8_t i = 0; i < a_PD.GetComponentsNbr(); i++) {
            const auto dataType = a_PD.GetDataType(i);
            assert(dataType != Core::DataType::None);
            switch (dataType) {
            case Core::DataType::Uint8:
                funcs.at(i) = GetComponentUint8;
                break;
            case Core::DataType::Int8:
                funcs.at(i) = GetComponentInt8;
                break;
            case Core::DataType::Uint16:
                funcs.at(i) = GetComponentUint16;
                break;
            case Core::DataType::Int16:
                funcs.at(i) = GetComponentInt16;
                break;
            case Core::DataType::Uint32:
                funcs.at(i) = GetComponentUint32;
                break;
            case Core::DataType::Int32:
                funcs.at(i) = GetComponentInt32;
                break;
            case Core::DataType::Float16:
                funcs.at(i) = GetComponentFloat16;
                break;
            case Core::DataType::Float32:
                funcs.at(i) = GetComponentFloat32;
                break;
            }
        }
    }
    return funcs;
}

static inline void SetComponentNormalizedUint8(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<uint8_t*>(a_Bytes) = uint8_t(glm::clamp(a_Component, 0.f, 1.f) * float(UINT8_MAX));
}
static inline void SetComponentNormalizedInt8(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<int8_t*>(a_Bytes) = int8_t(glm::clamp(a_Component, 0.f, 1.f) * float(INT8_MAX));
}
static inline void SetComponentNormalizedUint16(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<uint16_t*>(a_Bytes) = uint16_t(glm::clamp(a_Component, 0.f, 1.f) * float(UINT16_MAX));
}
static inline void SetComponentNormalizedInt16(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<int16_t*>(a_Bytes) = int16_t(glm::clamp(a_Component, 0.f, 1.f) * float(INT16_MAX));
}
static inline void SetComponentUint8(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<uint8_t*>(a_Bytes) = uint8_t(a_Component);
}
static inline void SetComponentInt8(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<int8_t*>(a_Bytes) = int8_t(a_Component);
}
static inline void SetComponentUint16(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<uint16_t*>(a_Bytes) = uint16_t(a_Component);
}
static inline void SetComponentInt16(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<int16_t*>(a_Bytes) = int16_t(a_Component);
}
static inline void SetComponentUint32(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<uint32_t*>(a_Bytes) = uint32_t(a_Component);
}
static inline void SetComponentInt32(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<int32_t*>(a_Bytes) = int32_t(a_Component);
}
static inline void SetComponentFloat16(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<glm::detail::hdata*>(a_Bytes) = glm::detail::toFloat16(a_Component);
}
static inline void SetComponentFloat32(std::byte* a_Bytes, const float& a_Component) noexcept
{
    *reinterpret_cast<float*>(a_Bytes) = a_Component;
}

auto GetSetComponentFunc(const PixelDescriptor& a_PD)
{
    std::array<PixelDescriptor::SetComponentFunc, 4> funcs;
    if (a_PD.IsNormalized()) {
        for (uint8_t i = 0; i < a_PD.GetComponentsNbr(); i++) {
            const auto dataType = a_PD.GetDataType(i);
            assert(dataType != Core::DataType::None);
            assert(dataType != Core::DataType::Uint32 && "Uint32 textures cannot be normalized");
            assert(dataType != Core::DataType::Int32 && "Int32 textures cannot be normalized");
            assert(dataType != Core::DataType::Float16 && "Float16 textures cannot be normalized");
            assert(dataType != Core::DataType::Float32 && "Float32 textures cannot be normalized");
            switch (dataType) {
            case Core::DataType::Uint8:
                funcs.at(i) = SetComponentNormalizedUint8;
                break;
            case Core::DataType::Int8:
                funcs.at(i) = SetComponentNormalizedInt8;
                break;
            case Core::DataType::Uint16:
                funcs.at(i) = SetComponentNormalizedUint16;
                break;
            case Core::DataType::Int16:
                funcs.at(i) = SetComponentNormalizedInt16;
                break;
            }
        }
    } else {
        for (uint8_t i = 0; i < a_PD.GetComponentsNbr(); i++) {
            const auto dataType = a_PD.GetDataType(i);
            assert(dataType != Core::DataType::None);
            switch (dataType) {
            case Core::DataType::Uint8:
                funcs.at(i) = SetComponentUint8;
                break;
            case Core::DataType::Int8:
                funcs.at(i) = SetComponentInt8;
                break;
            case Core::DataType::Uint16:
                funcs.at(i) = SetComponentUint16;
                break;
            case Core::DataType::Int16:
                funcs.at(i) = SetComponentInt16;
                break;
            case Core::DataType::Uint32:
                funcs.at(i) = SetComponentUint32;
                break;
            case Core::DataType::Int32:
                funcs.at(i) = SetComponentInt32;
                break;
            case Core::DataType::Float16:
                funcs.at(i) = SetComponentFloat16;
                break;
            case Core::DataType::Float32:
                funcs.at(i) = SetComponentFloat32;
                break;
            }
        }
    }
    return funcs;
}

PixelDescriptor::PixelDescriptor(const PixelSizedFormat& a_Format)
    : _PixelSizedFormatHelper(a_Format)
{
    for (uint8_t i = 0; i < 4; i++)
        _PixelSize += GetDataTypeSize(i);
    SetComponent = GetSetComponentFunc(*this);
    GetComponent = GetGetComponentFunc(*this);
}

PixelColor PixelDescriptor::GetColorFromBytes(const std::vector<std::byte>& a_Bytes, const PixelSize& a_ImageSize, const PixelSize& a_PixelCoordinates) const
{
    auto pixelIndex { GetPixelIndex(a_ImageSize, a_PixelCoordinates) };
    auto pixelPtr { &a_Bytes.at(pixelIndex) };
    assert((pixelIndex + GetPixelSize()) <= a_Bytes.size() && "The pixel is out of bound");
    return GetColorFromBytes(pixelPtr);
}

PixelColor PixelDescriptor::GetColorFromBytes(const std::byte* a_Bytes) const
{
    assert(GetComponentsNbr() <= 4 && "Incorrect pixel type");
    PixelColor color { 0, 0, 0, 1 };
    for (uint8_t i = 0; i < GetComponentsNbr(); ++i) {
        color[i] = GetComponent[i](a_Bytes);
        a_Bytes += GetDataTypeSize(i);
    }
    return color;
}

void PixelDescriptor::SetColorToBytes(std::byte* a_Bytes, const PixelColor& a_Color) const
{
    SetColorToBytesRange(a_Bytes, a_Bytes + GetPixelSize(), a_Color);
}

void PixelDescriptor::SetColorToBytesRange(std::byte* a_Begin, std::byte* a_End, const PixelColor& a_Color) const
{
    assert(GetComponentsNbr() <= 4);
    const auto compNbr   = GetComponentsNbr();
    const auto pixelSize = GetPixelSize();
    std::byte colorValue[4][4]; // first convert color
    for (unsigned i = 0; i < compNbr; ++i)
        SetComponent[i](colorValue[i], a_Color[i]);
    while (a_Begin != a_End) {
        for (unsigned i = 0; i < compNbr; ++i) {
            const auto dataTypeSize = GetDataTypeSize(i);
            memcpy(a_Begin, colorValue[i], dataTypeSize);
            a_Begin += dataTypeSize;
        }
    }
}
}
