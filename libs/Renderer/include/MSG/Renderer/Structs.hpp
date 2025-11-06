#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Renderer.hpp>
#include <MSG/Renderer/Enums.hpp>

#include <any>
#include <string>

#include <glm/vec3.hpp>

#if MSG_RENDERER_BACKEND == MSG_RENDERER_OpenGL
namespace Platform {
class Ctx;
}
#endif // MSG_RENDERER_BACKEND == MSG_RENDERER_OpenGL

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace Msg::Renderer {
struct AutoExposureSettings {
    bool enabled          = true;
    float minLuminance    = 0.0001;
    float maxLuminance    = 1000.f;
    float key             = 0.5f;
    float adaptationSpeed = 1.0f;
};

struct ToneMappingSettings {
    AutoExposureSettings autoExposure;
    float exposure   = 0.0f; // exposure offset, ignored if auto exposure is enabled
    float saturation = 0.0f; // saturation offset
    float contrast   = 0.0f; // contrast offset
    float gamma      = 2.2f;
};
struct SSAOSettings {
    QualitySetting quality = QualitySetting::High;
    float radius           = 1.f; // the maximum world distance to display SSAO
    float strength         = 1.f;
};
// This is used to pass settings to Renderer at construction and during runtime
struct RendererSettings {
    float internalResolution     = 1.f;
    bool enableTAA               = true;
    QualitySetting shadowQuality = QualitySetting::High;
    glm::uvec3 volumetricFogRes  = GetDefaultVolumetricFogRes(QualitySetting::High);
    RendererMode mode            = RendererMode::Deferred;
    SSAOSettings ssao;
    ToneMappingSettings toneMapping;
};

struct CreateRendererInfo {
    std::string name            = "";
    uint32_t applicationVersion = 0;
#if MSG_RENDERER_BACKEND == MSG_RENDERER_OpenGL
    /**
     * @brief [OPTIONAL] used to store a rendering context manually created beforehand.
     * The Renderer will take ownership of this context
     */
    Platform::Ctx* context = nullptr;
#endif // MSG_RENDERER_BACKEND == MSG_RENDERER_OpenGL
};
}

namespace Msg::RenderBuffer {
struct CreateRenderBufferInfo {
    uint32_t width  = 0;
    uint32_t height = 0;
};
}
