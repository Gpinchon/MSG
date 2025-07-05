#pragma once

#include <MSG/Renderer/OGL/Subsystems/SubsystemInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>

#include <MSG/PageFile.hpp>

#include <VirtualTexturing.glsl>

#include <chrono>
#include <gcem.hpp>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include <glm/vec2.hpp>

namespace MSG {
class OGLContext;
class OGLTexture2DArray;
class OGLFrameBuffer;
class OGLProgram;
template <typename>
class OGLTypedBufferArray;
}

namespace MSG::Renderer {
class SparseTexture;
}

namespace MSG::Renderer {
constexpr std::chrono::milliseconds VTPollingRate = std::chrono::milliseconds(250u); // query used pages only 4 times per seconds
constexpr float VTPagesUploadBudget               = 0.5; // upload budget in Mo
class TexturingSubsystem : public SubsystemInterface {
public:
    TexturingSubsystem(Renderer::Impl& a_Renderer);
    void Update(Renderer::Impl& a_Renderer, const SubsystemLibrary& a_Subsystems) override;

    OGLContext ctx; // we need to place it here for destruction order

    std::shared_ptr<OGLTypedBufferArray<GLSL::VTMaterialInfo>> feedbackMaterialsBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFeedbackOutput>> feedbackOutputBuffer;

private:
    std::chrono::system_clock::time_point _lastUpdate;
    OGLFence _feedbackFence;
    OGLCmdBuffer _feedbackCmdBuffer;
    std::shared_ptr<OGLProgram> _feedbackProgram;
    std::shared_ptr<OGLFrameBuffer> _feedbackFB;
    std::unordered_set<std::shared_ptr<SparseTexture>> _managedTextures;
    std::mutex _commitsMutex;
};
}