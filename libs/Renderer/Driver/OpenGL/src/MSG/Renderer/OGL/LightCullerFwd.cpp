#include <MSG/Camera.hpp>
#include <MSG/ECS/Registry.hpp>
#include <MSG/Entity/Camera.hpp>
#include <MSG/Light/PunctualLight.hpp>
#include <MSG/OGLBuffer.hpp>
#include <MSG/Renderer/OGL/Components/LightData.hpp>
#include <MSG/Renderer/OGL/LightCullerFwd.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Renderer/OGL/ShaderCompiler.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Transform.hpp>

#include <VTFS.glsl>

#include <GL/glew.h>

template <typename LightType>
void MSG::Renderer::LightCullerFwd::_PushLight(const LightType& a_LightData, GLSL::FwdIBL&, GLSL::FwdShadowsBase&)
{
    vtfs.PushLight(a_LightData);
}

template <>
void MSG::Renderer::LightCullerFwd::_PushLight(const Component::LightIBLData& a_LightData, GLSL::FwdIBL& a_IBL, GLSL::FwdShadowsBase& a_Shadows)
{
    if (a_IBL.count < FWD_LIGHT_MAX_IBL) [[likely]] {
        auto& index      = a_IBL.count;
        auto& light      = a_IBL.lights[index];
        light.commonData = a_LightData.commonData;
        light.halfSize   = a_LightData.halfSize;
        for (uint8_t i = 0; i < a_LightData.irradianceCoefficients.size(); i++)
            light.irradianceCoefficients[i] = GLSL::vec4(a_LightData.irradianceCoefficients[i], 1.f);
        a_IBL.count++;
    }
}

template <>
inline void MSG::Renderer::LightCullerFwd::_PushLight(const Component::LightData& a_LightData, GLSL::FwdIBL& a_IBL, GLSL::FwdShadowsBase& a_Shadows)
{
    if (a_LightData.shadow.has_value() && a_LightData.shadow->cast && a_Shadows.count < FWD_LIGHT_MAX_SHADOWS) [[unlikely]] {
        auto& index             = a_Shadows.count;
        auto& shadow            = a_Shadows.shadows[index];
        shadow.light            = std::visit([this, shadow](auto& a_Data) mutable { return *reinterpret_cast<const GLSL::LightBase*>(&a_Data); }, a_LightData);
        shadow.projection       = a_LightData.shadow->projection.GetData();
        shadows.textures[index] = a_LightData.shadow->texture;
        a_Shadows.count++;
        return;
    }
    return std::visit([this, a_IBL, a_Shadows](auto& a_Data) mutable { _PushLight(a_Data, a_IBL, a_Shadows); }, a_LightData);
}

MSG::Renderer::LightCullerFwd::LightCullerFwd(Renderer::Impl& a_Renderer)
    : _renderer(a_Renderer)
    , vtfs(a_Renderer)
    , ibl(_renderer.context)
    , shadows(_renderer.context)
{
}

void MSG::Renderer::LightCullerFwd::operator()(Scene* a_Scene, const std::shared_ptr<OGLBuffer>& a_CameraUBO)
{
    vtfs.Prepare();
    GLSL::FwdIBL iblBuffer            = ibl.UBO.GetData();
    GLSL::FwdShadowsBase shadowBuffer = shadows.UBO.GetData();
    iblBuffer.count                   = 0;
    shadowBuffer.count                = 0;
    for (auto& entity : a_Scene->GetVisibleEntities().lights) {
        _PushLight(entity.GetComponent<Component::LightData>(), iblBuffer, shadowBuffer);
    }
    vtfs.Cull(a_CameraUBO);
    ibl.UBO.SetData(iblBuffer);
    shadows.UBO.SetData(shadowBuffer);
    if (ibl.UBO.needsUpdate)
        _renderer.uboToUpdate.push_back(ibl.UBO);
    if (shadows.UBO.needsUpdate)
        _renderer.uboToUpdate.push_back(shadows.UBO);
}
