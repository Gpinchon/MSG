#pragma once

#include <MSG/ECS/Registry.hpp>
#include <MSG/OGLTypedBuffer.hpp>

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
class OGLTextureCubemap;
class OGLTexture;
class OGLFrameBuffer;
}

namespace MSG::Renderer {
class Impl;
}

namespace MSG::Renderer::Component {
struct LightIBLData {
    GLSL::LightCommon commonData;
    glm::vec3 halfSize;
    std::shared_ptr<OGLTextureCubemap> specular;
    std::array<glm::vec3, 16> irradianceCoefficients;
};
struct LightShadowData {
    LightShadowData(Renderer::Impl& a_Rdr, const PunctualLight& a_SGLight, const MSG::Transform& a_Transform);
    bool cast = false;
    std::shared_ptr<OGLTypedBuffer<GLSL::Camera>> projBuffer;
    std::shared_ptr<OGLTexture> texture;
    std::shared_ptr<OGLFrameBuffer> frameBuffer;
};
using LightDataBase = std::variant<GLSL::LightPoint, GLSL::LightSpot, GLSL::LightDirectional, LightIBLData>;
struct LightData : LightDataBase {
    using LightDataBase::LightDataBase;
    LightData(Renderer::Impl& a_Renderer,
        const PunctualLight& a_SGLight,
        const ECS::DefaultRegistry::EntityRefType& a_Entity);
    auto GetType() const { return index(); }
    std::optional<LightShadowData> shadow;
};
}