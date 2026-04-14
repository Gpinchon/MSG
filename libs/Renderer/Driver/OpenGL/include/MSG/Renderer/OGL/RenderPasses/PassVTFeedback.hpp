#pragma once

#include <MSG/Renderer/RenderPassInterface.hpp>

#include <MSG/Renderer/OGL/Subsystems/TexturingSubsystem.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>
#include <MSG/OGLRenderPassInfo.hpp>
#include <MSG/WorkerThread.hpp>

#include <chrono>
#include <memory>
#include <unordered_map>

#include <glm/vec3.hpp>

namespace Msg::Renderer::GLSL {
struct VTFeedbackSettings;
struct VTFeedbackMaterialInfo;
}

namespace Msg {
class OGLFrameBuffer;
class OGLProgram;
template <typename>
class OGLTypedBufferArray;
template <typename>
class OGLTypedBuffer;
class OGLVertexArray;
}

namespace Msg::Renderer {
constexpr std::chrono::milliseconds VTPollingRate = std::chrono::milliseconds(250u); // query used pages only 4 times per seconds
class PassVTFeedback : public RenderPassInterface {
public:
    PassVTFeedback(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const RenderPassesLibrary& a_RenderPasses) override;
    void Render(Impl& a_Renderer) override;

private:
    void _RecordCmdBuffer(Renderer::Impl& a_Renderer);
    void _CreateFeedbackBuffers(const glm::uvec2& a_BufferRes);
    std::shared_ptr<OGLVertexArray> _LoadPrimitive(Renderer::Primitive& a_rPrimitive);
    OGLContext _ctx;
    bool _renderNeeded      = false;
    bool _feedbackRequested = false;
    std::chrono::system_clock::time_point _lastUpdate;
    std::unordered_map<std::shared_ptr<OGLVertexArray>, std::shared_ptr<OGLVertexArray>> _VAOs;
    std::shared_ptr<OGLTypedBuffer<GLSL::VTFeedbackSettings>> _feedbackSettingsBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFeedbackMaterialInfo>> _feedbackMaterialsBuffer;
    std::shared_ptr<OGLProgram> _feedbackProgramSkinned;
    std::shared_ptr<OGLProgram> _feedbackProgram;
    std::shared_ptr<OGLFrameBuffer> _feedbackFB;
    OGLFence _feedbackFence { true };
    OGLRenderPassInfo _feedbackRenderPass;
    OGLCmdBuffer _feedbackCmdBuffer;
    VTFeedbackData _feedbackData;
};
}