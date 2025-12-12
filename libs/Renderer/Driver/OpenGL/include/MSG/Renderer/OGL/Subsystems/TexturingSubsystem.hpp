#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>

#include <MSG/PageFile.hpp>

#include <VirtualTexturing.glsl>

#include <atomic>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <unordered_set>

#include <glm/vec2.hpp>

namespace Msg {
class OGLContext;
class OGLTexture2DArray;
class OGLFrameBuffer;
class OGLProgram;
template <typename>
class OGLTypedBufferArray;
}

namespace Msg::Renderer {
class SparseTexture;
}

namespace Msg::Renderer {
constexpr std::chrono::milliseconds SparseTexturePollingRate      = std::chrono::milliseconds(250u); // query used pages only 4 times per seconds
constexpr std::chrono::milliseconds SparseTextureUploadTimeBudget = std::chrono::milliseconds(16u); // try maintaining at least 60 fps
class TexturingSubsystem : public SubsystemInterface {
public:
    TexturingSubsystem(Renderer::Impl& a_Renderer);
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;

    OGLContext ctx; // we need to place it here for destruction order

    std::shared_ptr<OGLTypedBufferArray<GLSL::VTMaterialInfo>> feedbackMaterialsBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTFeedbackOutput>> feedbackOutputBuffer;

private:
    std::chrono::system_clock::time_point _lastUpdate;
    OGLFence _feedbackFence;
    OGLCmdBuffer _feedbackCmdBuffer;
    std::shared_ptr<OGLProgram> _feedbackProgramSkinned;
    std::shared_ptr<OGLProgram> _feedbackProgram;
    std::shared_ptr<OGLFrameBuffer> _feedbackFB;
    std::unordered_set<std::shared_ptr<SparseTexture>> _managedTextures;
    std::atomic<bool> _needsUpdate = true;
};
}