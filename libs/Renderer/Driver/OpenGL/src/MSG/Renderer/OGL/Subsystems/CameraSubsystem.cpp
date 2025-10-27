#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>

#include <MSG/Camera.hpp>
#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Tools/Halton.hpp>

#include <Camera.glsl>

static inline auto GetTemporalJitter(const glm::vec2& a_InternalRes, const uint8_t& a_FrameIndex)
{
    auto halton = Msg::Tools::Halton23<256>(a_FrameIndex + 1) * 2.f - 1.f;
    return halton / a_InternalRes * 0.5f; // the jitter amount should go bellow the threshold of velocity buffer
}

static inline auto ApplyTemporalJitter(glm::mat4 a_ProjMat, const glm::vec2& a_InternalRes, const uint8_t& a_FrameIndex)
{
    auto offset = GetTemporalJitter(a_InternalRes, a_FrameIndex);
    a_ProjMat[2][0] += offset.x;
    a_ProjMat[2][1] += offset.y;
    return a_ProjMat;
}

Msg::Renderer::CameraSubsystem::CameraSubsystem(Renderer::Impl& a_Renderer)
    : buffer(std::make_shared<OGLTypedBuffer<GLSL::CameraUBO>>(a_Renderer.context))
{
}

void Msg::Renderer::CameraSubsystem::Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

void Msg::Renderer::CameraSubsystem::Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity)
{
}

void Msg::Renderer::CameraSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems)
{
    auto& activeScene                = *a_Renderer.activeScene;
    auto& currentCamera              = activeScene.GetCamera();
    auto& camera                     = currentCamera.GetComponent<Camera>();
    auto resolution                  = glm::vec2((*a_Renderer.activeRenderBuffer)->width, (*a_Renderer.activeRenderBuffer)->height) * a_Renderer.settings.internalResolution;
    GLSL::CameraUBO cameraUBOData    = buffer->Get();
    cameraUBOData.previous           = cameraUBOData.current;
    cameraUBOData.current.position   = currentCamera.GetComponent<Msg::Transform>().GetWorldPosition();
    cameraUBOData.current.projection = camera.projection.GetMatrix();
    cameraUBOData.current.zNear      = camera.projection.GetZNear();
    cameraUBOData.current.zFar       = camera.projection.GetZFar();
    if (a_Renderer.settings.enableTAA) {
        cameraUBOData.current.jitter     = GetTemporalJitter(resolution, uint8_t(a_Renderer.frameIndex));
        cameraUBOData.current.projection = ApplyTemporalJitter(cameraUBOData.current.projection, resolution, uint8_t(a_Renderer.frameIndex));
    } else
        cameraUBOData.current.jitter = glm::vec2(0, 0);
    cameraUBOData.current.view = glm::inverse(currentCamera.GetComponent<Msg::Transform>().GetWorldTransformMatrix());
    buffer->Set(cameraUBOData);
    buffer->Update();
}