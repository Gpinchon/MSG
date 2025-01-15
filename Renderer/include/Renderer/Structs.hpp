#pragma once

////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include <any>
#include <optional>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////
// Class definition
////////////////////////////////////////////////////////////////////////////////
namespace TabGraph::Renderer {
enum class RendererMode {
    Forward,
    Deferred
};

// This is used to pass settings to Renderer at construction and during runtime
struct RendererSettings {
    float scale       = 1.f;
    bool enableTAA    = true;
    RendererMode mode = RendererMode::Forward;
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
