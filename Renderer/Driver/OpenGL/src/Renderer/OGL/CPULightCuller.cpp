#include <Renderer/OGL/CPULightCuller.hpp>
#include <Renderer/OGL/Components/LightData.hpp>
#include <Renderer/OGL/RAII/Buffer.hpp>
#include <Renderer/OGL/RAII/Wrapper.hpp>
#include <Renderer/OGL/Renderer.hpp>

#ifdef _WIN32
#include <Renderer/OGL/Win32/Context.hpp>
#elif defined __linux__
#include <Renderer/OGL/Unix/Context.hpp>
#endif // WIN32

#include <ECS/Registry.hpp>

#include <SG/Component/Camera.hpp>
#include <SG/Component/Light/PunctualLight.hpp>
#include <SG/Component/Transform.hpp>
#include <SG/Entity/Camera.hpp>
#include <SG/Scene/Scene.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

namespace TabGraph::Renderer {
static GLSL::LightBase ConvertLight(const GLSL::LightPoint& a_Light, std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX>&, unsigned&)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const GLSL::LightSpot& a_Light, std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX>&, unsigned&)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const GLSL::LightDirectional& a_Light, std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX>&, unsigned&)
{
    return *reinterpret_cast<const GLSL::LightBase*>(&a_Light);
}

static GLSL::LightBase ConvertLight(const Component::LightIBLData& a_Light, std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX>& a_IBLSamplers, unsigned& a_IBLightIndex)
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

static GLSL::LightBase ConvertLight(const Component::LightData& a_LightData, std::array<std::shared_ptr<RAII::TextureCubemap>, VTFS_IBL_MAX>& a_IBLSamplers, unsigned& a_IBLightIndex)
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
    void operator()(const TabGraph::Tools::ComputeInputs& a_Input)
    {
        const auto clusterIndex = a_Input.workGroupSize.x * a_Input.workGroupID.x + a_Input.localInvocationID.x;
        auto& lightCluster      = clusters[clusterIndex];
        lightCluster.count      = 0;
        for (uint32_t lightIndex = 0; lightIndex < lights.count; ++lightIndex) {
            const auto& light = lights.lights[lightIndex];
            if (LightIntersectsAABB(
                    light, view, proj,
                    lightCluster.aabb.minPoint, lightCluster.aabb.maxPoint)
                && lightCluster.count < VTFS_CLUSTER_MAX) {
                lightCluster.index[lightCluster.count] = lightIndex;
                lightCluster.count++;
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
    , GPUlightsBuffer(RAII::MakePtr<RAII::Buffer>(_context, sizeof(_lights), &_lights, GL_DYNAMIC_STORAGE_BIT))
    , GPUclusters(RAII::MakePtr<RAII::Buffer>(_context, sizeof(_clusters), _clusters, GL_DYNAMIC_STORAGE_BIT))
{
    auto vtfsClusters = GLSL::GenerateVTFSClusters();
    for (uint32_t i = 0; i < VTFS_CLUSTER_COUNT; ++i) {
        _clusters[i] = vtfsClusters[i];
    }
}

void TabGraph::Renderer::CPULightCuller::operator()(SG::Scene* a_Scene)
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
    auto cameraView = SG::Camera::GetViewMatrix(a_Scene->GetCamera());
    auto cameraProj = a_Scene->GetCamera().GetComponent<SG::Component::Camera>().projection.GetMatrix();
    CullingFunctor functor(cameraView, cameraProj, _lights, _clusters.data());
    _compute.Dispatch(functor, { VTFS_CLUSTER_COUNT / VTFS_LOCAL_SIZE, 1, 1 });
    _context.PushCmd([this] {
        _compute.Wait();
        glNamedBufferSubData(
            GPUlightsBuffer->handle,
            0, sizeof(_lights), &_lights);
        glNamedBufferSubData(
            GPUclusters->handle,
            0, sizeof(_clusters), _clusters.data());
    });
}

}
