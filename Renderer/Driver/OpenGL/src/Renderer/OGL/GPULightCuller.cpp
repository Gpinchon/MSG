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
GLSL::LightBase GetGLSLLight(
    const SG::Component::PunctualLight& a_SGLight,
    const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
    GLSL::LightBase glslLight;
    glslLight.commonData.position = SG::Node::GetWorldPosition(a_Entity);
    std::visit([&glslLight](auto& a_Data) {
        glslLight.commonData.intensity = a_Data.intensity;
        glslLight.commonData.range     = a_Data.range;
        glslLight.commonData.color     = a_Data.color;
        glslLight.commonData.falloff   = a_Data.falloff;
        glslLight.commonData.priority  = a_Data.priority;
    },
        a_SGLight);
    switch (a_SGLight.GetType()) {
    case SG::Component::LightType::Point:
        glslLight.commonData.type = LIGHT_TYPE_POINT;
        break;
    case SG::Component::LightType::Directional: {
        glslLight.commonData.type = LIGHT_TYPE_DIRECTIONAL;
        auto& dirLight            = reinterpret_cast<GLSL::LightDirectional&>(glslLight);
        dirLight.halfSize         = std::get<SG::Component::LightDirectional>(a_SGLight).halfSize;
    } break;
    case SG::Component::LightType::Spot: {
        glslLight.commonData.type = LIGHT_TYPE_SPOT;
        auto& spot                = reinterpret_cast<GLSL::LightSpot&>(glslLight);
        spot.direction            = SG::Node::GetForward(a_Entity);
        spot.innerConeAngle       = std::get<SG::Component::LightSpot>(a_SGLight).innerConeAngle;
        spot.outerConeAngle       = std::get<SG::Component::LightSpot>(a_SGLight).outerConeAngle;
    } break;
    default:
        break;
    }
    return glslLight;
}

bool LightIntersects(
    IN(GLSL::LightBase) a_Light,
    IN(glm::mat4x4) a_MVP,
    IN(glm::vec3) a_AABBMin,
    IN(glm::vec3) a_AABBMax)
{
    if (a_Light.commonData.type == LIGHT_TYPE_POINT || a_Light.commonData.type == LIGHT_TYPE_SPOT) {
        glm::vec3 lightPosition = a_Light.commonData.position;
        float lightRadius       = a_Light.commonData.range;
        GLSL::ProjectSphereToNDC(lightPosition, lightRadius, a_MVP);
        // closest point on the AABB to the sphere center
        glm::vec3 closestPoint = glm::clamp(lightPosition, a_AABBMin, a_AABBMax);
        glm::vec3 diff         = closestPoint - lightPosition;
        // squared distance between the sphere center and closest point
        if (float distanceSquared = dot(diff, diff); !(distanceSquared <= lightRadius * lightRadius))
            return false;
        // if it's a spot and it's outside the AABB
        if (a_Light.commonData.type == LIGHT_TYPE_SPOT && closestPoint != lightPosition) {
            auto& spot                  = reinterpret_cast<const GLSL::LightSpot&>(a_Light);
            glm::vec3 ndcLightDirection = glm::normalize(a_MVP * glm::vec4(spot.direction, 0));
            glm::vec3 closestDir        = glm::normalize(lightPosition - closestPoint);
            float closestAngle          = glm::dot(closestDir, ndcLightDirection);
            return closestAngle <= spot.outerConeAngle;
        }
        return true;
    }
    return false;
}

GPULightCuller::GPULightCuller(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , _cullingProgram(a_Renderer.shaderCompiler.CompileProgram("VTFSCulling"))
{

    for (uint i = 0; i < GPULightCullerBufferNbr; i++) {
        _GPUclustersBuffers.at(i) = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSCluster) * VTFS_CLUSTER_COUNT, GLSL::GenerateVTFSClusters().data(), GL_NONE);
        _GPUlightsBuffers.at(i)   = RAII::MakePtr<RAII::Buffer>(_renderer.context, sizeof(GLSL::VTFSLightsBuffer), nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
        a_Renderer.context.PushCmd([this, lightBuffer = _GPUlightsBuffers.at(i), i] {
            auto bufferPtr             = glMapNamedBufferRange(*lightBuffer, 0, lightBuffer->size, GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_WRITE_BIT | GL_MAP_FLUSH_EXPLICIT_BIT);
            _GPULightsBufferPtrs.at(i) = reinterpret_cast<GLSL::VTFSLightsBuffer*>(bufferPtr);
        });
    }
    GPUlightsBuffer = _GPUlightsBuffers.at(0);
}

/**
 * @todo sort lights according to priority
 */
void GPULightCuller::operator()(SG::Scene* a_Scene)
{
    GPUlightsBuffer = _GPUlightsBuffers.at(_currentLightBuffer);
    GPUclusters     = _GPUclustersBuffers.at(_currentLightBuffer);
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
        auto entity     = registry->GetEntityRef(entityID);
        auto worldLight = GetGLSLLight(punctualLight, entity);
        if (LightIntersects(worldLight, MVP, cameraFrustum.minPoint, cameraFrustum.maxPoint)) {
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
