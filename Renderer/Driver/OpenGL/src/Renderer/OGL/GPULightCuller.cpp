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
    switch (a_SGLight.type) {
    case SG::Component::PunctualLight::Type::Point:
        a_GLSLLight.commonData.type = LIGHT_TYPE_POINT;
        break;
    case SG::Component::PunctualLight::Type::Directional: {
        a_GLSLLight.commonData.type = LIGHT_TYPE_DIRECTIONAL;
        auto& dirLight              = reinterpret_cast<GLSL::LightDirectional&>(a_GLSLLight);
        dirLight.halfSize           = a_SGLight.data.directional.halfSize;
    } break;
    case SG::Component::PunctualLight::Type::Spot: {
        a_GLSLLight.commonData.type = LIGHT_TYPE_SPOT;
        auto& spot                  = reinterpret_cast<GLSL::LightSpot&>(a_GLSLLight);
        spot.direction              = SG::Node::GetForward(a_Entity);
        spot.innerConeAngle         = a_SGLight.data.spot.innerConeAngle;
        spot.outerConeAngle         = a_SGLight.data.spot.outerConeAngle;
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
            auto bufferPtr             = glMapNamedBufferRange(*buffer, 0, buffer->size, GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
            _GPULightsBufferPtrs.at(i) = reinterpret_cast<GLSL::VTFSLightsBuffer*>(bufferPtr);
        });
    }
    GPUlightsBuffer = _GPUlightsBuffers.at(0);
}

void GPULightCuller::operator()(SG::Scene* a_Scene)
{
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
        auto entity                     = registry->GetEntityRef(entityID);
        worldLight.commonData.position  = SG::Node::GetWorldPosition(entity);
        worldLight.commonData.color     = punctualLight.data.base.color;
        worldLight.commonData.range     = punctualLight.data.base.range;
        worldLight.commonData.intensity = punctualLight.data.base.intensity;
        worldLight.commonData.falloff   = punctualLight.data.base.falloff;
        GetGLSLLight(worldLight, punctualLight, entity);
        GLSL::vec3 lightPosition = worldLight.commonData.position;
        float lightRadius        = worldLight.commonData.range;
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
    _renderer.context.PushCmd([this, lightCount = lights.count] {
        auto lightBufferFlushSize = (sizeof(GLSL::LightBase) * lightCount) + offsetof(GLSL::VTFSLightsBuffer, lights);
        glFlushMappedNamedBufferRange(*GPUlightsBuffer, 0, lightBufferFlushSize);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, *_renderer.forwardCameraUBO.buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, *GPUlightsBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, *GPUclusters);
        glUseProgram(*_cullingProgram);
        glDispatchCompute(VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        //  unbind objects
        glUseProgram(0);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, 0);
    });
    GPUlightsBuffer     = _GPUlightsBuffers.at(_currentLightBuffer);
    _currentLightBuffer = (++_currentLightBuffer) % _GPUlightsBuffers.size();
}

}
