#pragma once

#include <ECS/Registry.hpp>

#include <Lights.glsl>

#include <array>
#include <memory>
#include <variant>

#include <glm/vec3.hpp>

namespace TabGraph::SG::Component {
struct PunctualLight;
}

namespace TabGraph::Renderer {
class Impl;
}

namespace TabGraph::Renderer::RAII {
class TextureCubemap;
}

namespace TabGraph::Renderer::Component {
struct LightIBLData {
    GLSL::LightCommon commonData;
    glm::vec3 halfSize;
    std::shared_ptr<RAII::TextureCubemap> specular;
    std::array<glm::vec3, 16> irradianceCoefficients;
};
using LightDataBase = std::variant<GLSL::LightPoint, GLSL::LightSpot, GLSL::LightDirectional, LightIBLData>;
struct LightData : LightDataBase {
    using LightDataBase::LightDataBase;
    LightData(Renderer::Impl& a_Renderer,
        const SG::Component::PunctualLight& a_SGLight,
        const ECS::DefaultRegistry::EntityRefType& a_Entity);
    auto GetType() const { return index(); }
};
}