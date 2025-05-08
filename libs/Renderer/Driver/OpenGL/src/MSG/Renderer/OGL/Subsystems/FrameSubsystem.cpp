#include <MSG/Renderer/OGL/Subsystems/FrameSubsystem.hpp>

#include <MSG/Renderer/OGL/RenderBuffer.hpp>
#include <MSG/Renderer/OGL/Renderer.hpp>

#include <MSG/OGLTexture2D.hpp>
#include <MSG/OGLTypedBuffer.hpp>

#include <FrameInfo.glsl>

MSG::Renderer::FrameSubsystem::FrameSubsystem(Renderer::Impl& a_Renderer)
    : buffer(std::make_shared<OGLTypedBuffer<GLSL::FrameInfo>>(a_Renderer.context))
{
}

void MSG::Renderer::FrameSubsystem::Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems)
{
    GLSL::FrameInfo frameInfo;
    frameInfo.width      = (*a_Renderer.activeRenderBuffer)->width;
    frameInfo.height     = (*a_Renderer.activeRenderBuffer)->height;
    frameInfo.frameIndex = a_Renderer.frameIndex;
    buffer->Set(frameInfo);
    buffer->Update();
}