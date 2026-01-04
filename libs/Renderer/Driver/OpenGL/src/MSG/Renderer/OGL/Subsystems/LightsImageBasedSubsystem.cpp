#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLSampler.hpp>
#include <MSG/OGLTextureCube.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/Components/LightImageBasedData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/Subsystems/LightsImageBasedSubsystem.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

static constexpr Msg::OGLSamplerParameters GetSamplerParameters()
{
    Msg::OGLSamplerParameters parameters;
    parameters.minFilter       = GL_LINEAR_MIPMAP_LINEAR;
    parameters.magFilter       = GL_LINEAR;
    parameters.seamlessCubemap = true;
    return parameters;
}

Msg::Renderer::LightsImageBasedSubsystem::LightsImageBasedSubsystem(Renderer::Impl& a_Renderer)
    : iblSpecSampler(std::make_shared<OGLSampler>(a_Renderer.context, GetSamplerParameters()))
{
}

void Msg::Renderer::LightsImageBasedSubsystem::Load(
    Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (!a_Entity.HasComponent<PunctualLight>() || !a_Entity.HasComponent<Transform>())
        return; // this entity doesn't have punctual light or transform
    if (a_Entity.HasComponent<Renderer::LightImageBasedData>())
        return; // already loaded
    auto& punctualLight = a_Entity.GetComponent<PunctualLight>();
    if (punctualLight.GetType() != LightType::IBL)
        return; // this light is not image based
    auto& transform = a_Entity.GetComponent<Transform>();
    a_Entity.AddComponent<Renderer::LightImageBasedData>(a_Renderer, iblSpecSampler, punctualLight, transform);
}

void Msg::Renderer::LightsImageBasedSubsystem::Unload(
    Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    if (!a_Entity.HasComponent<Renderer::LightImageBasedData>())
        return;
    a_Entity.RemoveComponent<Renderer::LightImageBasedData>();
}

void Msg::Renderer::LightsImageBasedSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& activeScene         = a_Renderer.activeScene;
    auto& registry            = *activeScene->GetRegistry();
    const auto& visibleLights = activeScene->GetVisibleEntities().lights;
    std::vector<LightImageBasedData*> ibls;
    for (auto& entityID : visibleLights) {
        auto entity         = registry.GetEntityRef(entityID);
        auto& punctualLight = entity.GetComponent<PunctualLight>();
        auto& transform     = entity.GetComponent<Transform>();
        if (punctualLight.GetType() == LightType::IBL) {
            if (!entity.HasComponent<LightImageBasedData>())
                Load(a_Renderer, entity);
            auto& ibl = entity.GetComponent<LightImageBasedData>();
            ibl.Update(a_Renderer, iblSpecSampler, punctualLight, transform);
            ibls.emplace_back(&ibl);
        } else if (entity.HasComponent<LightImageBasedData>())
            Unload(a_Renderer, entity); // Light type must have changed
    }
    count = ibls.size();
    textureSamplers.clear();
    textureSamplers.reserve(count);
    if (buffer == nullptr || buffer->GetCount() < count)
        buffer = std::make_shared<OGLTypedBufferArray<GLSL::LightIBL>>(a_Renderer.context, count);
    for (uint32_t i = 0; i < count; i++) {
        const auto& ibl = *ibls[i];
        textureSamplers.emplace_back(ibl.textureSampler);
        buffer->Set(i, ibl);
    }
    buffer->Update();
}