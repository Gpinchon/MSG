#include <Renderer/OGL/GPULightCuller.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Program.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>
#include <Renderer/OGL/ShaderCompiler.hpp>
#include <Renderer/OGL/Unix/Context.hpp>

#include <ECS/Registry.hpp>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

namespace TabGraph::Renderer {

void GetGLSLLight(
    GLSL::LightBase& a_GLSLLight,
    const SG::Component::PunctualLight& a_SGLight,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    switch (a_SGLight.GetType()) {
    case SG::Component::LightType::Point:
        a_GLSLLight.commonData.type = LIGHT_TYPE_POINT;
        break;
    case SG::Component::LightType::Directional: {
        a_GLSLLight.commonData.type = LIGHT_TYPE_DIRECTIONAL;
        auto& dirLight              = reinterpret_cast<GLSL::LightDirectional&>(a_GLSLLight);
        dirLight.halfSize           = std::get<SG::Component::LightDirectional>(a_SGLight).halfSize;
    } break;
    case SG::Component::LightType::Spot: {
        a_GLSLLight.commonData.type = LIGHT_TYPE_SPOT;
        auto& spot                  = reinterpret_cast<GLSL::LightSpot&>(a_GLSLLight);
        spot.direction              = SG::Node::GetForward(a_Entity);
        spot.innerConeAngle         = std::get<SG::Component::LightSpot>(a_SGLight).innerConeAngle;
        spot.outerConeAngle         = std::get<SG::Component::LightSpot>(a_SGLight).outerConeAngle;
    } break;
    default:
        break;
    }
}

GPULightCuller::GPULightCuller(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
    , GPUclusters(RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE))
{

    for (uint i = 0; i < GPULightCullerBufferNbr; i++) {
        auto buffer             = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        _GPUlightsBuffers.at(i) = buffer;
        a_Renderer.context.PushImmediateCmd([this, buffer, i] {
            auto bufferPtr             = glMapNamedBufferRange(*buffer, 0, buffer->size, GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
            _GPULightsBufferPtrs.at(i) = reinterpret_cast<GLSL::VTFSLightsBuffer*>(bufferPtr);
        });
    }
    GPUlightsBuffer = _GPUlightsBuffers.at(0);
}

void GPULightCuller::operator()(SG::Scene* a_Scene)
{
    GPUlightsBuffer = _GPUlightsBuffers.at(_currentLightBuffer);
    auto& lights    = *_GPULightsBufferPtrs.at(_currentLightBuffer);
    auto registry   = a_Scene->GetRegistry();
    auto cameraView = SG::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
    auto MVP        = cameraProj * cameraView;
    GLSL::VTFSClusterAABB cameraFrustum;
    cameraFrustum.minPoint = { -1, -1, -1 };
    cameraFrustum.maxPoint = { 1, 1, 1 };
    auto registryView      = registry->GetView<SG::Component::PunctualLight, SG::Component::Transform>();
    // pre-cull lights
    lights.count = 0;
    for (const auto& [entityID, punctualLight, transform] : registryView) {
        GLSL::LightBase worldLight;
        auto entity                    = registry->GetEntityRef(entityID);
        worldLight.commonData.position = SG::Node::GetWorldPosition(entity);
        std::visit([&worldLight](auto& a_Data) {
            worldLight.commonData.intensity = a_Data.intensity;
            worldLight.commonData.range     = a_Data.range;
            worldLight.commonData.color     = a_Data.color;
            worldLight.commonData.falloff   = a_Data.falloff;
        },
            punctualLight);
        GetGLSLLight(worldLight, punctualLight, entity);
        GLSL::vec3 lightPosition = worldLight.commonData.position;
        float lightRadius        = worldLight.commonData.range;
        if (punctualLight.GetType() == SG::Component::LightType::Spot) {
            auto& spotDir = reinterpret_cast<GLSL::LightSpot&>(worldLight).direction;
            lightPosition = lightPosition + (spotDir * (lightRadius / 2.f));
            lightRadius   = lightRadius / 2.f;
        }
        GLSL::ProjectSphereToNDC(lightPosition, lightRadius, MVP);
        if (GLSL::SphereIntersectsAABB(
                lightPosition, lightRadius,
                cameraFrustum.minPoint, cameraFrustum.maxPoint)) {
            lights.lights[lights.count] = worldLight;
            lights.count++;
            if (lights.count == VTFS_BUFFER_MAX)
                break;
        }
    }
    _renderer.context.PushCmd([cameraUBO          = _renderer.fwdCameraUBO.buffer,
                                  cullingProgram  = _cullingProgram,
                                  GPUlightsBuffer = GPUlightsBuffer,
                                  GPUclusters     = GPUclusters,
                                  lightCount      = lights.count] {
        auto lightBufferFlushSize = (sizeof(GLSL::LightBase) * lightCount) + offsetof(GLSL::VTFSLightsBuffer, lights);
        glFlushMappedNamedBufferRange(*GPUlightsBuffer, 0, lightBufferFlushSize);
        glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, *cameraUBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *GPUlightsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, *GPUclusters);
        glUseProgram(*cullingProgram);
        glDispatchCompute(VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1);
        glMemoryBarrierByRegion(GL_SHADER_STORAGE_BARRIER_BIT);
        //  unbind objects
        glUseProgram(0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    });
    _currentLightBuffer = (++_currentLightBuffer) % _GPUlightsBuffers.size();
}

}
