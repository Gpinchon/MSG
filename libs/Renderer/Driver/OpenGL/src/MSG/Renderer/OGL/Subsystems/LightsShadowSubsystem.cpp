#include <MSG/Camera.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/Scene.hpp>

#include <MSG/Renderer/OGL/Components/LightShadowData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsShadowSubsystem.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsVTFSSubsystem.hpp> //for light type conversion

#include <MSG/OGLBindlessTextureSampler.hpp>
#include <MSG/OGLPipeline.hpp>
#include <MSG/OGLTexture.hpp>

#include <Lights.glsl>

#include <GL/glew.h>

Msg::Renderer::LightsShadowSubsystem::LightsShadowSubsystem(Renderer::Impl& a_Renderer)
{
}

void Msg::Renderer::LightsShadowSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (!a_Entity.HasComponent<PunctualLight>() || !a_Entity.HasComponent<Transform>())
        return; // entity doesn't have punctual light or transform
    if (a_Entity.HasComponent<LightShadowData>())
        return; // already loaded
    auto& punctualLight = a_Entity.GetComponent<PunctualLight>();
    auto& transform     = a_Entity.GetComponent<Transform>();
    if (!punctualLight.CastsShadow())
        return; // this light doesn't cast shadow
    a_Entity.AddComponent<LightShadowData>(a_Renderer);
}

void Msg::Renderer::LightsShadowSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (!a_Entity.HasComponent<LightShadowData>())
        return; // not loaded
    a_Entity.RemoveComponent<LightShadowData>();
}

struct TempShadowData {
    const Msg::Renderer::LightShadowData* data;
    const Msg::SceneVisibleLight* light;
};

void Msg::Renderer::LightsShadowSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& activeScene         = a_Renderer.activeScene;
    auto& registry            = *activeScene->GetRegistry();
    const auto& visibleLights = activeScene->GetVisibleEntities().lights;
    countViewports            = 0;
    countCasters              = 0;
    std::vector<TempShadowData> shadows;
    for (auto& visibleLight : visibleLights) {
        auto entity         = registry.GetEntityRef(visibleLight);
        auto& punctualLight = entity.GetComponent<PunctualLight>();
        if (punctualLight.CastsShadow()) {
            if (!entity.HasComponent<LightShadowData>())
                Load(a_Renderer, entity);
            auto& shadowData = entity.GetComponent<LightShadowData>();
            shadowData.Update(a_Renderer, punctualLight.GetType(), punctualLight.GetShadowSettings(), visibleLight.viewports.size());
            shadows.emplace_back(TempShadowData { &shadowData, &visibleLight });
            countViewports += visibleLight.viewports.size();
            countCasters++;
        } else if (entity.HasComponent<LightShadowData>()) [[unlikely]]
            Unload(a_Renderer, entity); // this light must have stopped casting shadows
    }
    textureSamplers.clear();
    textureSamplers.reserve(countCasters);
    if (countCasters == 0)
        return;
    if (bufferCasters == nullptr || bufferCasters->GetCount() < countCasters)
        bufferCasters = std::make_shared<OGLTypedBufferArray<GLSL::ShadowCaster>>(a_Renderer.context, countCasters);
    if (bufferViewports == nullptr || bufferViewports->GetCount() < countViewports)
        bufferViewports = std::make_shared<OGLTypedBufferArray<GLSL::Camera>>(a_Renderer.context, countViewports);
    uint32_t vewportIndex = 0;
    for (uint32_t casterI = 0; casterI < countCasters; casterI++) {
        auto& tempData      = shadows[casterI];
        auto& entityRef     = registry.GetEntityRef(*tempData.light);
        auto& punctualLight = entityRef.GetComponent<PunctualLight>();
        auto& transform     = entityRef.GetComponent<Transform>();
        auto shadowCaster   = bufferCasters->Get(casterI);
        textureSamplers.emplace_back(tempData.data->textureSampler);
        shadowCaster.samplerHandle = *tempData.data->textureSampler;
        shadowCaster.blurRadius    = punctualLight.GetShadowSettings().blurRadius;
        shadowCaster.bias          = punctualLight.GetShadowSettings().bias;
        shadowCaster.normalBias    = punctualLight.GetShadowSettings().normalBias;
        shadowCaster.minDepth      = tempData.data->minDepth;
        shadowCaster.maxDepth      = tempData.data->maxDepth;
        shadowCaster.viewportIndex = vewportIndex;
        shadowCaster.viewportCount = tempData.light->viewports.size();
        shadowCaster.lightType     = GetGLSLLightType(punctualLight.GetType());
        bufferCasters->Set(casterI, shadowCaster);
        for (uint32_t vpI = 0; vpI < shadowCaster.viewportCount; vpI++) {
            auto& sgViewport        = tempData.light->viewports[vpI];
            auto glslViewport       = bufferViewports->Get(casterI + vpI);
            auto zNear              = sgViewport.projection.GetZNear();
            auto zFar               = sgViewport.projection.GetZFar();
            glslViewport.position   = transform.GetWorldPosition();
            glslViewport.projection = sgViewport.projection;
            glslViewport.view       = sgViewport.viewMatrix;
            glslViewport.zNear      = zNear;
            glslViewport.zFar       = glm::isinf(zFar) ? 1000000.f : zFar;
            bufferViewports->Set(vewportIndex, glslViewport);
            vewportIndex++;
        }
    }
    bufferCasters->Update();
    bufferViewports->Update();
}
