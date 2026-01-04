#pragma once

#include <MSG/Component.hpp>
#include <MSG/ECS/Registry.hpp>

#include <Lights.glsl>

#include <memory>

namespace Msg {
class OGLTexture;
class OGLSampler;
class OGLBindlessTextureSampler;
class Transform;
struct PunctualLight;
}

namespace Msg::Renderer {
class Impl;
}

namespace Msg::Renderer {
struct LightImageBasedData : GLSL::LightIBL, Msg::Component {
    LightImageBasedData(
        Renderer::Impl& a_Renderer,
        const std::shared_ptr<OGLSampler>& a_Sampler,
        const Msg::PunctualLight& a_Light,
        const Transform& a_Transform);
    void Update(
        Renderer::Impl& a_Renderer,
        const std::shared_ptr<OGLSampler>& a_Sampler,
        const Msg::PunctualLight& a_Light,
        const Transform& a_Transform);
    std::shared_ptr<OGLBindlessTextureSampler> textureSampler;
};
}