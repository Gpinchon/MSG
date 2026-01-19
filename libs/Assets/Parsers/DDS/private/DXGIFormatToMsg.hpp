#pragma once

#include <DirectXTex.h>
#include <MSG/Pixel/SizedFormat.hpp>

namespace Msg {
Msg::PixelSizedFormat ToMsg(const DXGI_FORMAT& a_Format)
{
    using enum PixelSizedFormat;
    switch (a_Format) {
    case DXGI_FORMAT_UNKNOWN:
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
        return Float32_RGBA;
        break;
    case DXGI_FORMAT_R32G32B32A32_UINT:
        return Uint32_RGBA;
        break;
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return Int32_RGBA;
        break;
    case DXGI_FORMAT_R32G32B32_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R32G32B32_FLOAT:
        return Float32_RGB;
        break;
    case DXGI_FORMAT_R32G32B32_UINT:
        return Uint32_RGB;
        break;
    case DXGI_FORMAT_R32G32B32_SINT:
        return Int32_RGB;
        break;
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
        return Float16_RGBA;
        break;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        return Uint16_NormalizedRGBA;
        break;
    case DXGI_FORMAT_R16G16B16A16_UINT:
        return Uint16_RGBA;
        break;
    case DXGI_FORMAT_R16G16B16A16_SNORM:
        return Int16_NormalizedRGBA;
        break;
    case DXGI_FORMAT_R16G16B16A16_SINT:
        return Int16_RGBA;
        break;
    case DXGI_FORMAT_R32G32_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R32G32_FLOAT:
        return Float32_RG;
        break;
    case DXGI_FORMAT_R32G32_UINT:
        return Uint32_RG;
        break;
    case DXGI_FORMAT_R32G32_SINT:
        return Int32_RG;
        break;
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return Uint8_NormalizedRGBA;
        break;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R8G8B8A8_UINT:
        return Uint8_RGBA;
        break;
    case DXGI_FORMAT_R8G8B8A8_SNORM:
        return Int16_NormalizedRGBA;
        break;
    case DXGI_FORMAT_R8G8B8A8_SINT:
        return Int16_RGBA;
        break;
    case DXGI_FORMAT_R16G16_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R16G16_FLOAT:
        return Float16_RG;
        break;
    case DXGI_FORMAT_R16G16_UNORM:
        return Uint16_NormalizedRG;
        break;
    case DXGI_FORMAT_R16G16_UINT:
        return Uint16_RG;
        break;
    case DXGI_FORMAT_R16G16_SNORM:
        return Int16_NormalizedRG;
        break;
    case DXGI_FORMAT_R16G16_SINT:
        return Int16_RG;
        break;
    case DXGI_FORMAT_R32_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_D32_FLOAT:
        return Depth32F;
        break;
    case DXGI_FORMAT_R32_FLOAT:
        return Float32_R;
        break;
    case DXGI_FORMAT_R32_UINT:
        return Uint32_R;
        break;
    case DXGI_FORMAT_R32_SINT:
        return Int32_R;
        break;
    case DXGI_FORMAT_R24G8_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return Depth24_Stencil8;
        break;
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R8G8_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R8G8_UNORM:
        return Uint8_NormalizedRG;
        break;
    case DXGI_FORMAT_R8G8_UINT:
        return Uint8_RG;
        break;
    case DXGI_FORMAT_R8G8_SNORM:
        return Int8_NormalizedRG;
        break;
    case DXGI_FORMAT_R8G8_SINT:
        return Int8_RG;
        break;
    case DXGI_FORMAT_R16_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R16_FLOAT:
        return Float16_R;
        break;
    case DXGI_FORMAT_D16_UNORM:
        return Depth16;
        break;
    case DXGI_FORMAT_R16_UNORM:
        return Uint16_NormalizedR;
        break;
    case DXGI_FORMAT_R16_UINT:
        return Uint16_R;
        break;
    case DXGI_FORMAT_R16_SNORM:
        return Int16_NormalizedR;
        break;
    case DXGI_FORMAT_R16_SINT:
        return Int16_R;
        break;
    case DXGI_FORMAT_R8_TYPELESS:
        /// UNSUPPORTED
        break;
    case DXGI_FORMAT_R8_UNORM:
        return Uint8_NormalizedR;
        break;
    case DXGI_FORMAT_R8_UINT:
        return Uint8_R;
        break;
    case DXGI_FORMAT_R8_SNORM:
        return Int8_NormalizedR;
        break;
    case DXGI_FORMAT_R8_SINT:
        return Int8_R;
        break;
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R1_UNORM:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
    case DXGI_FORMAT_AYUV:
    case DXGI_FORMAT_Y410:
    case DXGI_FORMAT_Y416:
    case DXGI_FORMAT_NV12:
    case DXGI_FORMAT_P010:
    case DXGI_FORMAT_P016:
    case DXGI_FORMAT_420_OPAQUE:
    case DXGI_FORMAT_YUY2:
    case DXGI_FORMAT_Y210:
    case DXGI_FORMAT_Y216:
    case DXGI_FORMAT_NV11:
    case DXGI_FORMAT_AI44:
    case DXGI_FORMAT_IA44:
    case DXGI_FORMAT_P8:
    case DXGI_FORMAT_A8P8:
    case DXGI_FORMAT_B4G4R4A4_UNORM:
    case DXGI_FORMAT_P208:
    case DXGI_FORMAT_V208:
    case DXGI_FORMAT_V408:
    case DXGI_FORMAT_SAMPLER_FEEDBACK_MIN_MIP_OPAQUE:
    case DXGI_FORMAT_SAMPLER_FEEDBACK_MIP_REGION_USED_OPAQUE:
    case DXGI_FORMAT_FORCE_UINT:
        /// UNSUPPORTED
        break;
    default:
        break;
    }
    return PixelSizedFormat::Unknown;
}
}
