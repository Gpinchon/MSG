#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <MSG/Renderer.hpp>
#include <MSG/Renderer/Enums.hpp>

#include <any>
#include <optional>
#include <string>
#include <variant>

#include <glm/vec3.hpp>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace MSG::Renderer {
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
};

struct CreateRendererInfo {
    std::string name            = "";
    uint32_t applicationVersion = 0;
};

struct CreateRenderBufferInfo {
    uint32_t width  = 0;
    uint32_t height = 0;
};
}
