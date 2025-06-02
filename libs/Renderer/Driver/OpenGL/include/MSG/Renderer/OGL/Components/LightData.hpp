#pragma once

#include <MSG/ECS/Registry.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Scene.hpp>

#include <Camera.glsl>
#include <Lights.glsl>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <memory>
#include <optional>
#include <variant>

namespace MSG {
struct PunctualLight;
class OGLTextureCube;
class OGLTexture;
class OGLFrameBuffer;
class Transform;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer::Component {
struct LightIBLData {
    GLSL::LightCommon commonData;
    glm::vec3 halfSize;
    std::shared_ptr<OGLTextureCube> specular;
    std::array<glm::vec3, 16> irradianceCoefficients;
};
struct LightShadowData {
    LightShadowData(Renderer::Impl& a_Rdr, const PunctualLight& a_SGLight, const MSG::Transform& a_Transform);
    void Update(
        Renderer::Impl& a_Renderer,
        ECS::DefaultRegistry& a_Registry,
        const ECS::DefaultRegistry::EntityIDType& a_EntityID,
        const std::vector<SceneShadowViewport>& a_Viewports);
    std::vector<GLSL::Camera> projections;
    float blurRadius;
    float bias;
    std::shared_ptr<OGLTexture> textureDepth;
    std::vector<std::shared_ptr<OGLFrameBuffer>> frameBuffers;
};
using LightDataBase = std::variant<GLSL::LightPoint, GLSL::LightSpot, GLSL::LightDirectional, LightIBLData>;
struct LightData : LightDataBase {
    using LightDataBase::LightDataBase;
    LightData(
        Renderer::Impl& a_Renderer,
        ECS::DefaultRegistry& a_Registry,
        const ECS::DefaultRegistry::EntityIDType& a_EntityID);
    void Update(
        Renderer::Impl& a_Renderer,
        ECS::DefaultRegistry& a_Registry,
        const ECS::DefaultRegistry::EntityIDType& a_EntityID);
    auto GetType() const { return index(); }
    std::optional<LightShadowData> shadow;
};
}