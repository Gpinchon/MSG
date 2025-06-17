#include <MSG/Renderer/OGL/Subsystems/CameraSubsystem.hpp>

#include <MSG/Camera.hpp>
#include <MSG/OGLTypedBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>
#include <MSG/Scene.hpp>
#include <MSG/Tools/Halton.hpp>

#include <Camera.glsl>

static inline auto ApplyTemporalJitter(glm::mat4 a_ProjMat, const uint8_t& a_FrameIndex)
{
    // the jitter amount should go bellow the threshold of velocity buffer
    constexpr float f16lowest = 1 / 1024.f;
    constexpr float offset    = f16lowest * 0.25f;
    auto halton               = (MSG::Tools::Halton23<256>(a_FrameIndex) * 2.f - 1.f) * offset;
    a_ProjMat[2][0] += halton.x;
    a_ProjMat[2][1] += halton.y;
    return a_ProjMat;
}

MSG::Renderer::CameraSubsystem::CameraSubsystem(Renderer::Impl& a_Renderer)
    : buffer(std::make_shared<OGLTypedBuffer<GLSL::CameraUBO>>(a_Renderer.context))
{
}

void MSG::Renderer::CameraSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    auto& activeScene                = *a_Renderer.activeScene;
    auto& currentCamera              = activeScene.GetCamera();
    auto& camera                     = currentCamera.GetComponent<Camera>();
    GLSL::CameraUBO cameraUBOData    = buffer->Get();
    cameraUBOData.previous           = cameraUBOData.current;
    cameraUBOData.current.position   = currentCamera.GetComponent<MSG::Transform>().GetWorldPosition();
    cameraUBOData.current.projection = camera.projection.GetMatrix();
    cameraUBOData.current.zNear      = camera.projection.GetZNear();
    cameraUBOData.current.zFar       = camera.projection.GetZFar();
    if (a_Renderer.enableTAA)
        cameraUBOData.current.projection = ApplyTemporalJitter(cameraUBOData.current.projection, uint8_t(a_Renderer.frameIndex));
    cameraUBOData.current.view = glm::inverse(currentCamera.GetComponent<MSG::Transform>().GetWorldTransformMatrix());
    buffer->Set(cameraUBOData);
    buffer->Update();
}