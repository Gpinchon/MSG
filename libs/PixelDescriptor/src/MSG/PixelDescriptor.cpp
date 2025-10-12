#include <MSG/Pixel/SizedFormatHelper.hpp>
#include <MSG/PixelDescriptor.hpp>

#include <bit>
#include <cstring>

#include <glm/glm.hpp>

#define STB_DXT_IMPLEMENTATION
#define STB_DXT_STATIC
#include <stb_dxt.h>

namespace Msg {
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

size_t PixelDescriptor::GetPixelBufferByteSize(const PixelSize& a_Size) const
{
    if (GetUnsizedFormat() == PixelUnsizedFormat::RGBA_DXT5) {
        constexpr size_t blockSize    = 16;
        constexpr glm::uvec3 blockDim = { 4, 4, 1 };
        const glm::uvec3 blockCount   = (a_Size + (blockDim - 1u)) / blockDim;
        const size_t blocksNbr        = blockCount.x * blockCount.y * blockCount.z;
        return blockSize * blocksNbr;
    } else
        return GetPixelSize() * a_Size.x * a_Size.y * a_Size.z;
}

std::array<std::byte, 16> Msg::PixelDescriptor::CompressBlock(const PixelColor* a_Colors) const
{
    std::array<std::byte, 16> ret;
    std::array<glm::u8vec4, 16> colors;
    for (uint8_t i = 0; i < 16; i++) {
        auto& color  = *(a_Colors + i);
        colors.at(i) = {
            glm::clamp(color.r, 0.f, 1.f) * 255,
            glm::clamp(color.g, 0.f, 1.f) * 255,
            glm::clamp(color.b, 0.f, 1.f) * 255,
            glm::clamp(color.a, 0.f, 1.f) * 255,
        };
    }
    stb_compress_dxt_block((uint8_t*)ret.data(), (const uint8_t*)colors.data(), true, STB_DXT_NORMAL);
    return ret;
}

#pragma pack(push, 1)
struct BC1Color {
    BC1Color& operator=(const glm::vec3& a_Col)
    {
        uint32_t red   = uint8_t(glm::clamp(a_Col.r, 0.f, 1.f) * 255) & 0b011111;
        uint32_t green = uint8_t(glm::clamp(a_Col.g, 0.f, 1.f) * 255) & 0b111111;
        uint32_t blue  = uint8_t(glm::clamp(a_Col.b, 0.f, 1.f) * 255) & 0b011111;
        u16bits        = blue | (green << 5) | (red << 11);
        return *this;
    }
    operator glm::u8vec3() const
    {
        uint32_t r = (u16bits >> 11U) & 0b011111;
        uint32_t g = (u16bits >> 5U) & 0b111111;
        uint32_t b = (u16bits >> 0U) & 0b011111;
        r          = (r << 3U) | (r >> 2U);
        g          = (g << 2U) | (g >> 4U);
        b          = (b << 3U) | (b >> 2U);
        return { r, g, b };
    }
    operator glm::vec3() const
    {
        uint32_t r = (u16bits >> 11U) & 0b011111;
        uint32_t g = (u16bits >> 5U) & 0b111111;
        uint32_t b = (u16bits >> 0U) & 0b011111;
        r          = (r << 3U) | (r >> 2U);
        g          = (g << 2U) | (g >> 4U);
        b          = (b << 3U) | (b >> 2U);
        return {
            float(r) / 255.f,
            float(g) / 255.f,
            float(b) / 255.f,
        };
    }
    uint16_t u16bits;
};
static_assert(sizeof(BC1Color) == 2);

struct BC1ColorBlock {
    BC1Color color_0;
    BC1Color color_1;
    uint32_t indexBlock;
    std::array<glm::u8vec3, 4> GetLutUI8() const;
    std::array<glm::vec3, 4> GetLut() const;
    uint8_t operator[](const uint8_t& a_Index) const;
};
static_assert(sizeof(BC1ColorBlock) == 8);

struct BC3AlphaBlock {
    uint8_t alpha_0 : 8;
    uint8_t alpha_1 : 8;
    uint8_t indexBlock[6];
    std::array<uint8_t, 8> GetLutUI8() const;
    std::array<float, 8> GetLut() const;
    uint8_t operator[](const uint8_t& a_Index) const;
};
static_assert(sizeof(BC3AlphaBlock) == 8);

struct BC3Block {
    BC3AlphaBlock alphaBlock;
    BC1ColorBlock colorBlock;
};
static_assert(sizeof(BC3Block) == 16);
static_assert(offsetof(BC3Block, alphaBlock) == 0);
static_assert(offsetof(BC3Block, colorBlock) == 8);
#pragma pack(pop)

std::array<glm::u8vec3, 4> Msg::BC1ColorBlock::GetLutUI8() const
{
    constexpr glm::u8vec3 black = { 0, 0, 0 };
    glm::u8vec3 col0            = color_0;
    glm::u8vec3 col1            = color_1;
    if (color_0.u16bits > color_1.u16bits) {
        return {
            col0,
            col1,
            glm::mix(col0, col1, 2.f / 3.f),
            glm::mix(col0, col1, 1.f / 3.f)
        };
    } else {
        return {
            col0,
            col1,
            glm::mix(col0, col1, 0.5f),
            black
        };
    }
    return {};
}

std::array<glm::vec3, 4> BC1ColorBlock::GetLut() const
{
    constexpr glm::vec3 black = { 0, 0, 0 };
    glm::vec3 col0            = color_0;
    glm::vec3 col1            = color_1;
    if (color_0.u16bits > color_1.u16bits) {
        return {
            col0,
            col1,
            2 / 3.f * col0 + 1 / 3.f * col1,
            1 / 3.f * col0 + 2 / 3.f * col1
        };
    } else {
        return {
            col0,
            col1,
            (col0 + col1) / 2.f,
            black
        };
    }
}
uint8_t BC1ColorBlock::operator[](const uint8_t& a_Index) const
{
    constexpr uint32_t bitCount = 2;
    constexpr uint32_t maxIndex = 0b11;
    const uint32_t bitShift     = (a_Index * bitCount);
    const uint32_t bitMask      = maxIndex << bitShift;
    return (indexBlock & bitMask) >> bitShift;
}

std::array<uint8_t, 8> BC3AlphaBlock::GetLutUI8() const
{
    if (alpha_0 > alpha_1) {
        return {
            alpha_0,
            alpha_1,
            glm::mix(alpha_0, alpha_1, 6.f / 7.f),
            glm::mix(alpha_0, alpha_1, 5.f / 7.f),
            glm::mix(alpha_0, alpha_1, 4.f / 7.f),
            glm::mix(alpha_0, alpha_1, 3.f / 7.f),
            glm::mix(alpha_0, alpha_1, 2.f / 7.f),
            glm::mix(alpha_0, alpha_1, 1.f / 7.f)
        };
    } else {
        return {
            alpha_0,
            alpha_1,
            glm::mix(alpha_0, alpha_1, 4 / 5.f),
            glm::mix(alpha_0, alpha_1, 3 / 5.f),
            glm::mix(alpha_0, alpha_1, 2 / 5.f),
            glm::mix(alpha_0, alpha_1, 1 / 5.f),
            0,
            1,
        };
    }
}

std::array<float, 8> BC3AlphaBlock::GetLut() const
{
    float alpha0 = alpha_0 / 255.f;
    float alpha1 = alpha_1 / 255.f;
    if (alpha0 > alpha1) {
        return {
            alpha0,
            alpha1,
            (6 / 7.f * alpha0 + 1 / 7.f * alpha1),
            (5 / 7.f * alpha0 + 2 / 7.f * alpha1),
            (4 / 7.f * alpha0 + 3 / 7.f * alpha1),
            (3 / 7.f * alpha0 + 4 / 7.f * alpha1),
            (2 / 7.f * alpha0 + 5 / 7.f * alpha1),
            (1 / 7.f * alpha0 + 6 / 7.f * alpha1)
        };
    } else {
        return {
            alpha0,
            alpha1,
            (4 / 5.f * alpha0 + 1 / 5.f * alpha1),
            (3 / 5.f * alpha0 + 2 / 5.f * alpha1),
            (2 / 5.f * alpha0 + 3 / 5.f * alpha1),
            (1 / 5.f * alpha0 + 4 / 5.f * alpha1),
            0,
            1,
        };
    }
}
uint8_t BC3AlphaBlock::operator[](const uint8_t& a_Index) const
{
    constexpr uint32_t bitCount = 3;
    constexpr uint32_t maxIndex = 0b111;
    const uint32_t bit_index    = a_Index * bitCount;
    const uint32_t byte_index   = bit_index >> 3;
    const uint32_t bit_ofs      = bit_index & 7;
    uint32_t v                  = indexBlock[byte_index];
    if (byte_index < (maxIndex - 1))
        v |= (indexBlock[byte_index + 1] << 8);
    return (v >> bit_ofs) & 7;
}

std::array<PixelColor, 16> Msg::PixelDescriptor::DecompressBlock(const std::byte* a_Block) const
{
    std::array<PixelColor, 16> ret;
    auto& bc3Block = *reinterpret_cast<const BC3Block*>(a_Block);
    auto alphaLut  = bc3Block.alphaBlock.GetLut();
    auto colorLut  = bc3Block.colorBlock.GetLut();
    for (uint8_t i = 0; i < 16; i++) {
        auto alphaIndex = bc3Block.alphaBlock[i];
        auto colorIndex = bc3Block.colorBlock[i];
        ret[i]          = {
            colorLut[colorIndex],
            alphaLut[alphaIndex]
        };
    }
    return ret;
}

std::array<glm::u8vec4, 16> Msg::PixelDescriptor::DecompressBlockToUI8(const std::byte* a_Block) const
{
    std::array<glm::u8vec4, 16> ret;
    auto& bc3Block = *reinterpret_cast<const BC3Block*>(a_Block);
    auto alphaLut  = bc3Block.alphaBlock.GetLutUI8();
    auto colorLut  = bc3Block.colorBlock.GetLutUI8();
    for (uint8_t i = 0; i < 16; i++) {
        auto alphaIndex = bc3Block.alphaBlock[i];
        auto colorIndex = bc3Block.colorBlock[i];
        ret[i]          = {
            colorLut[colorIndex],
            alphaLut[alphaIndex]
        };
    }
    return ret;
}
}
