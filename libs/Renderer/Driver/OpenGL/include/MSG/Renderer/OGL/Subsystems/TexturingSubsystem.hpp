#pragma once

#include <MSG/Renderer/SubsystemInterface.hpp>

#include <MSG/OGLCmdBuffer.hpp>
#include <MSG/OGLContext.hpp>
#include <MSG/OGLFence.hpp>

#include <MSG/PageFile.hpp>
// #include <MSG/ThreadPool.hpp>

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
constexpr std::chrono::milliseconds SparseTextureUploadTimeBudget = std::chrono::milliseconds(1u); // try maintaining at least 60 fps
class TexturingSubsystem : public SubsystemInterface {
public:
    TexturingSubsystem(Renderer::Impl& a_Renderer);
    void Load(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Unload(Renderer::Impl& a_Renderer, const ECS::DefaultRegistry::EntityRefType& a_Entity) override;
    void Update(Renderer::Impl& a_Renderer, const SubsystemsLibrary& a_Subsystems) override;
    OGLContext ctx; // we need to place it here for destruction order
    std::shared_ptr<OGLTypedBuffer<GLSL::VTFeedbackSettings>> feedbackSettingsBuffer;
    std::shared_ptr<OGLTypedBufferArray<GLSL::VTMaterialInfo>> feedbackMaterialsBuffer;

private:
    std::chrono::system_clock::time_point _lastUpdate;
    // ThreadPool _feedbackThreadPool            = { SAMPLERS_MATERIAL_COUNT };
    const glm::uvec3 _feedbackRes             = { 64, 64, SAMPLERS_MATERIAL_COUNT };
    std::vector<glm::vec3> _feedbackTexBuffer = std::vector<glm::vec3>(_feedbackRes.x * _feedbackRes.y * _feedbackRes.z, glm::vec3(0));
    OGLFence _feedbackFence { true };
    OGLCmdBuffer _feedbackCmdBuffer;
    std::shared_ptr<OGLProgram> _feedbackProgramSkinned;
    std::shared_ptr<OGLProgram> _feedbackProgram;
    std::shared_ptr<OGLFrameBuffer> _feedbackFB;
    std::unordered_set<std::shared_ptr<SparseTexture>> _managedTextures;
    std::atomic<bool> _needsUpdate = true;
};
}