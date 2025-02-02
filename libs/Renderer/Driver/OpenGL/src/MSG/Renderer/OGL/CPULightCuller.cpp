#include <MSG/Camera.hpp>
#include <MSG/Transform.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/Renderer/OGL/CPULightCuller.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

namespace MSG::Renderer {
template <typename LightType>
static GLSL::LightBase ConvertLight(const LightType& a_Light, std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX>&, unsigned&)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const Component::LightIBLData& a_Light, std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX>& a_IBLSamplers, unsigned& a_IBLightIndex)
{
    GLSL::LightIBL glslLight {};
    glslLight.commonData    = a_Light.commonData;
    glslLight.halfSize      = a_Light.halfSize;
    glslLight.specularIndex = a_IBLightIndex;
    for (auto i = 0; i < a_Light.irradianceCoefficients.size(); i++)
        glslLight.irradianceCoefficients[i] = glm::vec4(a_Light.irradianceCoefficients[i], 0);
    a_IBLSamplers[a_IBLightIndex] = a_Light.specular;
    ++a_IBLightIndex;
    return *reinterpret_cast<GLSL::LightBase*>(&glslLight);
}

static GLSL::LightBase ConvertLight(const Component::LightData& a_LightData, std::array<std::shared_ptr<OGLTextureCubemap>, VTFS_IBL_MAX>& a_IBLSamplers, unsigned& a_IBLightIndex)
{
    return std::visit([&a_IBLSamplers, &a_IBLightIndex](auto& a_Data) mutable {
        return ConvertLight(a_Data, a_IBLSamplers, a_IBLightIndex);
    },
        a_LightData);
}

struct CullingFunctor {
    CullingFunctor(
        const GLSL::mat4x4& a_View, const GLSL::mat4x4& a_Proj,
        const GLSL::VTFSLightsBuffer& a_Lights,
        GLSL::VTFSCluster* a_Clusters)
        : view(a_View)
        , proj(a_Proj)
        , lights(a_Lights)
        , clusters(a_Clusters)
    {
    }
    void operator()(const MSG::Tools::ComputeInputs& a_Input)
    {
        const auto clusterIndex      = a_Input.workGroupSize.x * a_Input.workGroupID.x + a_Input.localInvocationID.x;
        clusters[clusterIndex].count = 0;
        for (uint32_t lightIndex = 0; lightIndex < lights.count && clusters[clusterIndex].count < VTFS_CLUSTER_MAX; ++lightIndex) {
            const auto& light = lights.lights[lightIndex];
            if (LightIntersectsAABB(
                    light, view, proj,
                    clusters[clusterIndex].aabb.minPoint,
                    clusters[clusterIndex].aabb.maxPoint)) {
                clusters[clusterIndex].index[clusters[clusterIndex].count] = lightIndex;
                clusters[clusterIndex].count++;
            }
        }
    }
    const GLSL::mat4x4 view;
    const GLSL::mat4x4 proj;
    const GLSL::VTFSLightsBuffer& lights;
    GLSL::VTFSCluster* clusters;
};

CPULightCuller::CPULightCuller(Renderer::Impl& a_Renderer)
    : _context(a_Renderer.context)
    , GPUlightsBuffer(std::make_shared<OGLBuffer>(_context, sizeof(_lights), &_lights, GL_DYNAMIC_STORAGE_BIT))
    , GPUclusters(std::make_shared<OGLBuffer>(_context, sizeof(_clusters), _clusters.data(), GL_DYNAMIC_STORAGE_BIT))
{
    auto vtfsClusters = GLSL::GenerateVTFSClusters();
    for (uint32_t i = 0; i < VTFS_CLUSTER_COUNT; ++i)
        _clusters[i] = vtfsClusters[i];
}

void CPULightCuller::operator()(Scene* a_Scene)
{
    iblSamplers.fill(nullptr);
    unsigned iblLightCount = 0;
    _lights.count          = 0;
    for (auto& entity : a_Scene->GetVisibleEntities().lights) {
        const auto& lightData = entity.GetComponent<Component::LightData>();
        if (lightData.GetType() == LIGHT_TYPE_IBL && iblLightCount == VTFS_IBL_MAX)
            continue;
        _lights.lights[_lights.count] = ConvertLight(lightData, iblSamplers, iblLightCount);
        _lights.count++;
        if (_lights.count == VTFS_BUFFER_MAX)
            break;
    }
    auto cameraView = Entity::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<Camera>().projection.GetMatrix();
    CullingFunctor functor(cameraView, cameraProj, _lights, _clusters.data());
    _compute.Dispatch(functor, { VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1 });
    _compute.Wait();
    _context.PushCmd([this] {
        auto lightBufferSize = offsetof(GLSL::VTFSLightsBuffer, lights) + (sizeof(GLSL::LightBase) * _lights.count);
        glInvalidateBufferSubData(*GPUlightsBuffer, 0, lightBufferSize);
        glNamedBufferSubData(
            GPUlightsBuffer->handle,
            0, lightBufferSize, &_lights);
        auto clusterBufferSize = sizeof(_clusters);
        glInvalidateBufferSubData(*GPUclusters, 0, clusterBufferSize);
        glNamedBufferSubData(
            GPUclusters->handle,
            0, clusterBufferSize, _clusters.data());
    });
}

}
